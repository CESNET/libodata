#include "DataHub.h"

#include "BerkeleyDBStorage.h"
#include "CachedStorage.h"
#include "Connection.h"
#include "DataHubException.h"
#include "Directory.h"
#include "File.h"
#include "FileSystemNode.h"
#include "LRUCache.h"
#include "Product.h"
#include "ProductPath.h"
#include "ProductPlaceHolder.h"
#include "ProductStorage.h"
#include "RemoteFile.h"
#include "SearchQuery.h"
#include "TemporaryFile.h"
#include "XmlParser.h"
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include <glog/logging.h>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace OData {
struct DataHub::Impl {
  Impl(
      Connection& connection,
      const std::vector<std::string>& missions,
      std::shared_ptr<ProductStorage> product_storage,
      boost::filesystem::path tmp_path)
      : service_connection(connection),
        product_storage(std::move(product_storage)),
        filesytem_connection(service_connection.clone()),
        get_file_mutex(),
        missions(),
        data(std::make_shared<Directory>("root")),
        response_parser(),
        io_service(),
        timer(io_service, boost::posix_time::seconds(0)),
        timer_thread([&]() {
          this->timer.async_wait(
              [&](const boost::system::error_code&) { this->loadData(); });
          this->io_service.run();
        }),
        tmp_path(std::move(tmp_path)),
        tmp_file_name(0),
        file_cache(10) {
    for (auto& mission : missions) {
      this->missions[mission] = 0u;
    }
    timer_thread.detach();
  }

  ~Impl() {
    io_service.stop();
  }

  std::vector<std::shared_ptr<OData::Product>> getMissionProducts(
      const std::string& mission, std::uint32_t offset, std::uint32_t count) {
    auto products = service_connection.listProducts(
        {OData::SearchQuery::Keyword::PLATFORM, mission}, offset, count);
    for (auto& product : products) {
      if (product_storage->productExists(product->getId())) {
        LOG(INFO) << "Product '" << product->getId()
                  << "' fetched from local database";
        product = product_storage->getProduct(product->getId());
      } else {
        LOG(INFO) << "New product '" << product->getId() << "' found";
        auto manifest_path = product->getArchivePath();
        const auto manifest_filename = product->getManifestFilename();
        manifest_path.append(manifest_filename);
        try {
          auto manifest = service_connection.getFile(manifest_path);
          auto content =
              std::shared_ptr<Directory>(Directory::createRemoteStructure(
                  product->getArchivePath(),
                  product->getFilename(),
                  response_parser.parseManifest(manifest)));
          product->setArchiveStructure(
              std::move(content),
              std::make_shared<File>(
                  std::move(manifest_filename), std::move(manifest)));
        } catch (DataHubException& ex) {
          LOG(ERROR) << "Product details unavailable: " << ex.what();
        }
        product_storage->storeProduct(product);
      }
    }
    return products;
  }

  void loadData() {
    LOG(INFO) << "Load data timer expired";
    try {
      bool continue_synchronously;
      do {
        continue_synchronously = false;
        for (auto& mission : missions) {
          auto products =
              getMissionProducts(mission.first, mission.second, 100);
          continue_synchronously = !products.empty();
          mission.second += products.size();
          for (const auto& product : products) {
            data->appendProduct(
                std::make_shared<ProductPlaceHolder>(
                    product->getId(), product->getName(), product_storage),
                product->getPlatform(),
                product->getDate());
          }
        }
      } while (continue_synchronously);
    } catch (DataHubException& ex) {
      LOG(ERROR) << "Error occured during product discovery: " << ex.what();
    }
    this->timer.expires_from_now(boost::posix_time::seconds(60));
    this->timer.async_wait(
        [&](const boost::system::error_code&) { this->loadData(); });
  }

  boost::filesystem::path getNextTmpFile() {
    return tmp_path / ("odata" + std::to_string(tmp_file_name++) + ".tmp");
  }

  Connection& service_connection;
  std::shared_ptr<ProductStorage> product_storage;
  std::unique_ptr<Connection> filesytem_connection;
  std::mutex get_file_mutex;
  std::map<std::string, std::uint32_t> missions;
  std::shared_ptr<Directory> data;
  XmlParser response_parser;
  boost::asio::io_service io_service;
  boost::asio::deadline_timer timer;
  std::thread timer_thread;
  boost::filesystem::path tmp_path;
  std::uint8_t tmp_file_name;
  LRUCache<ProductPath, std::shared_ptr<TemporaryFile>> file_cache;
};

DataHub::DataHub(
    Connection& connection,
    const std::vector<std::string>& missions,
    boost::filesystem::path db_path,
    boost::filesystem::path tmp_path)
    : DataHub(
          connection,
          missions,
          std::make_shared<CachedStorage>(std::unique_ptr<ProductStorage>(
              new BerkeleyDBStorage(std::move(db_path)))),
          std::move(tmp_path)) {
}

DataHub::DataHub(
    Connection& connection,
    const std::vector<std::string>& missions,
    std::shared_ptr<ProductStorage> product_storage,
    boost::filesystem::path tmp_path)
    : pimpl(new Impl(
          connection,
          missions,
          std::move(product_storage),
          std::move(tmp_path))) {
}

std::vector<char> DataHub::getFile(
    const boost::filesystem::path& path,
    std::size_t offset,
    std::size_t length) {
  std::unique_lock<std::mutex> lock(pimpl->get_file_mutex);
  const auto file =
      static_cast<FileSystemNode*>(pimpl->data.get())->getFile(path);
  const auto local_file = std::dynamic_pointer_cast<const File>(file);
  if (local_file != nullptr) {
    auto data = local_file->getData();
    if (offset >= data.size()) {
      throw DataHubException(path.string(), "File offset out of range.");
    }
    const auto begin = data.begin() + offset;
    const auto end =
        offset + length < data.size() ? begin + length : data.end();
    return std::vector<char>(begin, end);
  }

  const auto remote_file = std::dynamic_pointer_cast<const ProductFile>(file);
  if (remote_file != nullptr) {
    ProductPath product_path = remote_file->getProductPath();
    std::shared_ptr<TemporaryFile> tmp_file;
    auto cached = pimpl->file_cache.get(product_path);
    if (cached.is_initialized()) {
      tmp_file = cached.get();
    } else {
      tmp_file = pimpl->filesytem_connection->getTemporaryFile(
          product_path, pimpl->getNextTmpFile());
      pimpl->file_cache.put(product_path, tmp_file);
    }
    return tmp_file->read(offset, length);
  }

  throw DataHubException(path.string(), "File read failed.");
}

DataHub::~DataHub() = default;

std::shared_ptr<FileSystemNode> DataHub::getData() {
  return pimpl->data;
}

} /* namespace OData */
