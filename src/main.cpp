#include "Connection.h"
#include "Directory.h"
#include "Product.h"
#include <cstdlib>
#include <iostream>
#include <memory>
#include <vector>

namespace {
std::vector<std::unique_ptr<OData::Product>> getMissionProducts(
    OData::Connection& connection, const std::string& mission, unsigned count) {
  auto products = connection.listProducts(mission, count);
  for (auto& product : products) {
    connection.updateProductDetails(*product);
  }
  return products;
}
} // namespace

int main(int argc, char** argv) {
  if (argc < 5) {
    std::cout << "try odata-client username password mission product_count"
              << std::endl;
    return 1;
  }
  OData::Connection connection("https://dhr1.cesnet.cz/", argv[1], argv[2]);
  auto filesystem = OData::Directory::createFilesystem(
      getMissionProducts(connection, "Sentinel-1", std::atoi(argv[4])));
  filesystem->appendProducts(
      getMissionProducts(connection, "Sentinel-2", std::atoi(argv[4])));
  filesystem->appendProducts(
      getMissionProducts(connection, "Sentinel-3", std::atoi(argv[4])));
  std::cout << *filesystem << std::endl;
  return 0;
}
