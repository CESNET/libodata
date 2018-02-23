#include "Connection.h"
#include "DataHub.h"
#include "FileSystemNode.h"
#include <cstdlib>
#include <iostream>
#include <memory>

int main(int argc, char** argv) {
  if (argc < 4) {
    std::cout << "try odata-client url username password" << std::endl;
    return 1;
  }
  OData::Connection connection(argv[1], argv[2], argv[3]);
  OData::DataHub hub(connection);
  const auto filesystem = hub.getData();
  std::cout << *filesystem << std::endl;
  return 0;
}
