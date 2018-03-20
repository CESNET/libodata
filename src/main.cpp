#include "DataHub.h"
#include "DataHubConnection.h"
#include "FileSystemNode.h"
#include <cstdlib>
#include <glog/logging.h>
#include <iostream>
#include <memory>

int main(int argc, char** argv) {
  if (argc < 4) {
    std::cout << "try odata-client url username password" << std::endl;
    return 1;
  }
  google::InitGoogleLogging(argv[0]);
  OData::DataHubConnection connection(argv[1], argv[2], argv[3]);
  OData::DataHub hub(connection, {"Sentinel-1", "Sentinel-2", "Sentinel-3"});
  const auto filesystem = hub.getData();
  std::cout << *filesystem << std::endl;
  return 0;
}
