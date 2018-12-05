#include "DataHub.h"

#include "BerkeleyDBStorage.h"
#include "CachedStorage.h"
#include "Connection.h"
#include "DataHubException.h"
#include "Directory.h"
#include "File.h"
#include "FileSystemNode.h"
#include "LRUCache.h"
#include "PathBuilder.h"
#include "Product.h"
#include "ProductAttribute.h"
#include "ProductPath.h"
#include "ProductPlaceHolder.h"
#include "ProductStorage.h"
#include "RemoteFile.h"
#include "SearchQuery.h"
#include "TemporaryFile.h"
#include "XmlParser.h"
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/thread.hpp>
#include <exception>
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
      boost::filesystem::path tmp_path,
      std::uint32_t cache_size,
      const PathBuilder& path_builder,
      std::uint32_t timeout_duration_ms)
      : service_connection(connection),
        product_storage(std::move(product_storage)),
        filesystem_connection(service_connection.clone()),
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
        path_builder(path_builder),
        file_cache(cache_size),
        stop(false),
        load_db(true),
        deleted_products_offset(0),
        timeout_duration_ms(timeout_duration_ms) {
    for (auto& mission : missions) {
      this->missions[mission] = 0u;
    }
  }

  ~Impl() {
    stop = true;
    io_service.stop();
    timer_thread.join();
  }

  bool initializeProductDetails(std::shared_ptr<Product> product) {
    auto manifest_path = product->getArchivePath();
    const auto manifest_filename = product->getManifestFilename();
    manifest_path.append(manifest_filename);
    try {
      auto manifest = service_connection.getFile(manifest_path);
      auto content =
          std::shared_ptr<Directory>(Directory::createRemoteStructure(
              product->getArchivePath(),
              *product->getAttribute(Attribute::FILENAME),
              response_parser.parseManifest(manifest)));
      product->setArchiveStructure(
          std::move(content),
          std::make_shared<File>(
              std::move(manifest_filename), std::move(manifest)));
      product_storage->storeProduct(product);
      return true;
    } catch (std::exception& ex) {
      LOG(ERROR) << "Product initialization failed: " << ex.what();
      return false;
    }
  }

  void removeDeletedProducts() {
    std::vector<std::string> deleted_products;
    do {
      deleted_products =
          service_connection.getDeletedProducts(deleted_products_offset);
      deleted_products_offset += deleted_products.size();
      LOG(INFO) << deleted_products.size() << " deleted product IDs downloaded";
      for (const auto& deleted_product : deleted_products) {
        if (product_storage->productExists(deleted_product)) {
          LOG(INFO) << "Removing deleted product: " << deleted_product;
          const auto product = product_storage->getProduct(deleted_product);
          auto parent = getFile(path_builder.createPath(*product));
          if (parent != nullptr) {
            parent->removeChild(product->getName());
          }
          try {
            product_storage->deleteProduct(deleted_product);
          } catch (std::exception& ex) {
            LOG(ERROR) << "Deleted product not removed: " << ex.what();
          }
        }
      }
    } while (!deleted_products.empty() && !stop);
  }

  void loadDatabase() {
    LOG(INFO) << "Loading database";
    load_db = false;
    auto iterator = product_storage->iterator();
    if (iterator == nullptr) {
      LOG(INFO) << "Loading database failed";
      return;
    }
    for (auto product = iterator->next(); product != nullptr;
         product = iterator->next()) {
      if (missions.find(product->getPlatform()) != missions.end()) {
        getFile(path_builder.createPath(*product), true)
            ->addChild(std::make_shared<ProductPlaceHolder>(
                product->getId(), product->getName(), product_storage));
      }
    }
  }

  void addNewProducts() {
    std::uint32_t continue_synchronously;
    do {
      continue_synchronously = 0;
      for (auto& mission : missions) {
        auto products = service_connection.listProducts(
            {Attribute::PLATFORM, mission.first}, mission.second, 100);
        continue_synchronously += products.size();
        mission.second += products.size();

        for (auto& product : products) {
          if (stop) {
            break;
          }
          if (product_storage->productExists(product->getId())) {
            LOG(INFO) << "Product '" << product->getId()
                      << "' already in local database";
          } else {
            LOG(INFO) << "New product '" << product->getId() << "' found";
            if (initializeProductDetails(product)) {
              getFile(path_builder.createPath(*product), true)
                  ->addChild(std::make_shared<ProductPlaceHolder>(
                      product->getId(), product->getName(), product_storage));
            }
          }
        }
      }
    } while (continue_synchronously > 0 && !stop);
  }

  void loadData() {
    LOG(INFO) << "Load data timer expired";
    try {
      removeDeletedProducts();
      if (load_db) {
        loadDatabase();
      }
      addNewProducts();
    } catch (std::exception& ex) {
      LOG(ERROR) << "Error occured during product discovery: " << ex.what();
    }
    this->timer.expires_from_now(
        boost::posix_time::milliseconds(timeout_duration_ms));
    this->timer.async_wait(
        [&](const boost::system::error_code&) { this->loadData(); });
  }

  boost::filesystem::path getNextTmpFile() {
    return tmp_path / ("odata" + std::to_string(tmp_file_name++) + ".tmp");
  }

  std::shared_ptr<FileSystemNode> getFile(
      const boost::filesystem::path& file_path,
      bool create_missing = false) const {
    if (file_path.empty()) {
      LOG(WARNING) << "Empty path requested";
      return nullptr;
    }
    auto path_it = file_path.begin();
    const auto end = file_path.end();
    if (path_it->has_root_directory()) {
      ++path_it;
    }
    if (path_it == end) {
      return data;
    }
    auto file = data->getFile(path_it, end);
    if (file == nullptr) {
      if (create_missing) {
        file = data;
        for (; path_it != end; ++path_it) {
          auto child = file->getChild(path_it->string());
          if (child == nullptr) {
            child = std::make_shared<Directory>(path_it->string());
            file->addChild(child);
          }
          file = child;
        }
      } else {
        LOG(INFO) << "Invalid file '" << file_path << "' requested";
      }
    }
    return file;
  }

  Connection& service_connection;
  std::shared_ptr<ProductStorage> product_storage;
  std::unique_ptr<Connection> filesystem_connection;
  std::mutex get_file_mutex;
  std::map<std::string, std::uint32_t> missions;
  std::shared_ptr<Directory> data;
  XmlParser response_parser;
  boost::asio::io_service io_service;
  boost::asio::deadline_timer timer;
  boost::thread timer_thread;
  boost::filesystem::path tmp_path;
  std::uint32_t tmp_file_name;
  const PathBuilder& path_builder;
  LRUCache<ProductPath, std::shared_ptr<TemporaryFile>> file_cache;
  std::atomic<bool> stop;
  bool load_db;
  std::uint32_t deleted_products_offset;
  std::uint32_t timeout_duration_ms;
}; // namespace

DataHub::DataHub(
    Connection& connection,
    const std::vector<std::string>& missions,
    boost::filesystem::path db_path,
    boost::filesystem::path tmp_path,
    std::uint32_t cache_size,
    const PathBuilder& path_builder)
    : DataHub(
          connection,
          missions,
          std::make_shared<CachedStorage>(
              std::make_unique<BerkeleyDBStorage>(std::move(db_path))),
          std::move(tmp_path),
          cache_size,
          path_builder,
          60000) {
}

DataHub::DataHub(
    Connection& connection,
    const std::vector<std::string>& missions,
    std::shared_ptr<ProductStorage> product_storage,
    boost::filesystem::path tmp_path,
    std::uint32_t cache_size,
    const PathBuilder& path_builder,
    std::uint32_t timeout_duration_ms)
    : pimpl(std::make_unique<Impl>(
          connection,
          missions,
          std::move(product_storage),
          std::move(tmp_path),
          cache_size,
          path_builder,
          timeout_duration_ms)) {
}

std::vector<char> DataHub::getFile(
    const boost::filesystem::path& path,
    std::size_t offset,
    std::size_t length) const {
  std::unique_lock<std::mutex> lock(pimpl->get_file_mutex);
  const auto file = pimpl->getFile(path);
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
      tmp_file = pimpl->filesystem_connection->getTemporaryFile(
          product_path, pimpl->getNextTmpFile());
      pimpl->file_cache.put(product_path, tmp_file);
    }
    return tmp_file->read(offset, length);
  }

  throw DataHubException(path.string(), "File read failed.");
}

DataHub::~DataHub() = default;

std::shared_ptr<FileSystemNode> DataHub::getData() noexcept {
  return pimpl->data;
}

std::shared_ptr<FileSystemNode> DataHub::getFile(
    const boost::filesystem::path& file_path) const {
  return pimpl->getFile(file_path);
}

} /* namespace OData */
