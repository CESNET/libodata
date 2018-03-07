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
#include <string>
#include <vector>

namespace OData {
struct DataHub::Impl {
  Impl(Connection& connection, std::vector<std::string> missions)
      : connection(connection), missions(std::move(missions)) {
  }

  std::vector<std::shared_ptr<OData::Product>> getMissionProducts(
      const std::string& mission, unsigned count) {
    auto products = connection.listProducts(
        {OData::SearchQuery::Keyword::PLATFORM, mission}, count);
    for (auto& product : products) {
      auto manifest_path = product->getProductPath();
      const auto manifest_filename = product->getManifestFilename();
      manifest_path.append(manifest_filename);
      auto manifest = connection.getFile(manifest_path);
      auto content =
          std::shared_ptr<Directory>(Directory::createRemoteStructure(
              product->getProductPath(),
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
    auto data = std::make_shared<Directory>("root");
    for (const auto& mission : missions) {
      data->appendProducts(getMissionProducts(mission, 100));
    }
    this->data = data;
  }

  Connection& connection;
  std::vector<std::string> missions;
  std::shared_ptr<FileSystemNode> data;
  XmlParser response_parser;
};

DataHub::DataHub(Connection& connection, std::vector<std::string> missions)
    : pimpl(new Impl(connection, std::move(missions))) {
}

std::vector<char> DataHub::getFile(const boost::filesystem::path& path) {
  const auto file = pimpl->data->getFile(path);
  const auto local_file = dynamic_cast<const File*>(file);
  const auto remote_file = dynamic_cast<const RemoteFile*>(file);
  if (local_file != nullptr) {
    return local_file->getData();
  } else if (remote_file) {
    return pimpl->connection.getFile(remote_file->getProductPath());
  }
  throw DataHubException(path.string(), "File read failed.");
}

DataHub::~DataHub() = default;

std::shared_ptr<FileSystemNode> DataHub::getData() {
  if (pimpl->data == nullptr) {
    pimpl->loadData();
  }
  return pimpl->data;
}

} /* namespace OData */
