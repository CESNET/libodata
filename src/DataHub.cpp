#include "DataHub.h"

#include "Connection.h"
#include "Directory.h"
#include "FileSystemNode.h"
#include "Product.h"
#include "SearchQuery.h"
#include <string>
#include <vector>

namespace OData {
struct DataHub::Impl {
  Impl(Connection& connection)
      : connection(connection),
        missions{"Sentinel-1", "Sentinel-2", "Sentinel-3"} {
  }

  std::vector<std::unique_ptr<OData::Product>> getMissionProducts(
      const std::string& mission, unsigned count) {
    auto products = connection.listProducts(
        {OData::SearchQuery::Keyword::PLATFORM, mission}, count);
    for (auto& product : products) {
      connection.updateProductFileStructure(*product);
    }
    return products;
  }

  std::unique_ptr<FileSystemNode> getData() {
    std::unique_ptr<Directory> data(new Directory("root"));
    for (const auto& mission : missions) {
      data->appendProducts(getMissionProducts(mission, 100));
    }
    return data;
  }

  Connection& connection;
  std::vector<std::string> missions;
};

DataHub::DataHub(Connection& connection) : pimpl(new Impl(connection)) {
}

DataHub::~DataHub() = default;

std::unique_ptr<FileSystemNode> DataHub::getData() {
  return pimpl->getData();
}

} /* namespace OData */
