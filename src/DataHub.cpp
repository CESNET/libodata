#include "DataHub.h"

#include "Connection.h"
#include "DataHubException.h"
#include "Directory.h"
#include "File.h"
#include "FileSystemNode.h"
#include "Product.h"
#include "ProductPath.h"
#include "RemoteFile.h"
#include "SearchQuery.h"
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
  Impl(Connection& connection, const std::vector<std::string>& missions)
      : service_connection(connection),
        filesytem_connection(service_connection.clone()),
        get_file_mutex(),
        missions(),
        data(std::make_shared<Directory>("root")),
        response_parser(),
        io_service(),
        timer(io_service, boost::posix_time::seconds(60)),
        timer_thread([&]() {
          this->timer.async_wait(
              [&](const boost::system::error_code&) { this->loadData(); });
          this->io_service.run();
        }) {
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
      auto manifest_path = product->getArchivePath();
      const auto manifest_filename = product->getManifestFilename();
      manifest_path.append(manifest_filename);
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
          data->appendProducts(std::move(products));
        }
      } while (continue_synchronously);
    } catch (DataHubException& ex) {
      LOG(ERROR) << "Error occured during product discovery: " << ex.what();
    }
    this->timer.expires_from_now(boost::posix_time::seconds(60));
    this->timer.async_wait(
        [&](const boost::system::error_code&) { this->loadData(); });
  }

  Connection& service_connection;
  std::unique_ptr<Connection> filesytem_connection;
  std::mutex get_file_mutex;
  std::map<std::string, std::uint32_t> missions;
  std::shared_ptr<Directory> data;
  XmlParser response_parser;
  boost::asio::io_service io_service;
  boost::asio::deadline_timer timer;
  std::thread timer_thread;
};

DataHub::DataHub(
    Connection& connection, const std::vector<std::string>& missions)
    : pimpl(new Impl(connection, missions)) {
}

std::vector<char> DataHub::getFile(const boost::filesystem::path& path) {
  std::unique_lock<std::mutex> lock(pimpl->get_file_mutex);
  const auto file =
      static_cast<FileSystemNode*>(pimpl->data.get())->getFile(path);
  const auto local_file = dynamic_cast<const File*>(file);
  const auto remote_file = dynamic_cast<const RemoteFile*>(file);
  const auto product_file = dynamic_cast<const Product*>(file);
  if (local_file != nullptr) {
    return local_file->getData();
  } else if (remote_file) {
    return pimpl->filesytem_connection->getFile(remote_file->getProductPath());
  } else if (product_file) {
    return pimpl->filesytem_connection->getFile(product_file->getProductPath());
  }
  throw DataHubException(path.string(), "File read failed.");
}

DataHub::~DataHub() = default;

std::shared_ptr<FileSystemNode> DataHub::getData() {
  return pimpl->data;
}

} /* namespace OData */
