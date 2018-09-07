#include "BerkeleyDBStorage.h"
#include "CachedStorage.h"
#include "Config.h"
#include "DataHub.h"
#include "DataHubConnection.h"
#include "DataHubException.h"
#include "FileSystemNode.h"
#include <boost/filesystem/path.hpp>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <glog/logging.h>
#include <iostream>
#include <limits>
#include <memory>
#include <string>

int main(int argc, char** argv) {
  OData::Config config(std::getenv("HOME"), argc, argv);
  if (config.printHelp()) {
    std::cout << config.getHelp() << std::endl;
    return 0;
  }
  if (config.printVersion()) {
    std::cout << config.getVersion() << std::endl;
    return 0;
  }
  if (!config.isValid()) {
    std::cerr << config.getErrorMessage() << std::endl;
    std::cout << config.getHelp() << std::endl;
    return 1;
  }
  google::InitGoogleLogging(argv[0]);
  LOG(INFO) << "Application started";
  OData::DataHubConnection connection(
      config.getUrl(), config.getUsername(), config.getPassword());
  boost::filesystem::path home(std::getenv("HOME"));
  OData::DataHub hub(
      connection,
      config.getMissions(),
      config.getDbPath(),
      config.getTmpPath(),
      config.getCacheSize(),
      config.getPathBuilder());
  while (true) {
    std::cout << "> ";
    std::string line;
    if (!getline(std::cin, line)) {
      break;
    }
    const auto separator = line.find(' ');
    std::string command, argument;
    if (separator == std::string::npos) {
      command = line;
    } else {
      command = line.substr(0, separator);
      argument = line.substr(separator + 1);
    }
    if (command == "ls") {
      std::shared_ptr<OData::FileSystemNode> filesystem;
      if (argument.empty()) {
        filesystem = hub.getData();
      } else {
        filesystem = hub.getFile(boost::filesystem::path(argument));
      }
      if (filesystem == nullptr) {
        std::cout << "Invalid path '" << argument << "'" << std::endl;
      } else {
        const auto dir = filesystem->readDir();
        if (dir.empty()) {
          std::cout << std::endl;
        }
        for (const auto& file : dir) {
          std::cout << file << std::endl;
        }
      }
    } else if (command == "get") {
      if (argument.empty()) {
        std::cout << "Invalid path" << std::endl;
      } else {
        try {
          std::size_t data_read = 0;
          const std::size_t chunk = 1000 * 1000;
          std::fstream file("out.file", std::fstream::out);
          while (true) {
            const auto data = hub.getFile(argument, data_read, chunk);
            if (data.empty()) {
              break;
            }
            data_read += data.size();
            file.write(data.data(), data.size());
          }
          file.flush();
        } catch (OData::DataHubException& exc) {
          std::cout << exc.what() << std::endl;
        }
      }
    } else if (command == "quit") {
      break;
    } else {
      std::cout << "Invalid command '" << command << "'" << std::endl;
    }
  }
  google::ShutdownGoogleLogging();
  return 0;
}
