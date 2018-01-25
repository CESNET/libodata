#include "Connection.h"
#include "Product.h"
#include <cstdlib>
#include <iostream>

int main(int argc, char** argv) {
  if (argc < 5) {
    std::cout << "try odata-client username password mission product_count"
              << std::endl;
    return 1;
  }
  OData::Connection connection("https://dhr1.cesnet.cz/", argv[1], argv[2]);
  auto files = connection.listProducts(argv[3], std::atoi(argv[4]));
  for (auto& file : files) {
    connection.updateProductDetails(file);
    std::cout << file << std::endl;
  }
  return 0;
}
