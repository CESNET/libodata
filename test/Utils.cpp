#include "Utils.h"

#include "Directory.h"
#include "File.h"
#include "FileSystemNode.h"
#include "PathBuilder.h"
#include "Product.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace OData {
namespace Test {
std::vector<char> readTestInstance(const std::string& filename) {
  std::fstream file("examples/" + filename, std::fstream::in);
  if (file.is_open()) {
    std::stringstream buffer;
    buffer << file.rdbuf();
    const auto data = buffer.str();
    return std::vector<char>(data.begin(), data.end());
  } else {
    throw std::invalid_argument("Cannot open file " + filename);
  }
}

std::unique_ptr<Product> createProduct(
    std::string id, std::string platform) noexcept {
  auto product = std::make_unique<Product>(std::map<std::string, std::string>{
      {"uuid", std::move(id)},
      {"identifier", "name"},
      {"beginposition", "2018-11-16T09:47:53.618"},
      {"filename", "filename"},
      {"platformname", std::move(platform)},
      {"producttype", "type"},
      {"size", "1KB"}});

  product->setArchiveStructure(
      Directory::createRemoteStructure(
          ProductPath("uuid", "filename"),
          "extracted",
          {{"manifest.xml", 10}, {"subdir/xxx", 20}, {"subdir2/yyy", 30}}),
      std::make_shared<File>(
          std::string("manifest.xml"), std::vector<char>{1, 0, 2, 3, 4}));
  return product;
}

std::unique_ptr<Directory> createFilesystem(
    std::vector<std::shared_ptr<Product>> products) noexcept {
  auto filesystem = std::make_unique<Directory>("root");
  PathBuilder path_builder("/${platformname}/${date}");
  for (auto product : products) {
    const auto path = path_builder.createPath(*product);
    filesystem->getOrCreateSubdirectory(++path.begin(), path.end())
        ->addChild(product);
  }
  return filesystem;
}

std::shared_ptr<FileSystemNode> testGetFile(
    const FileSystemNode& instance,
    const boost::filesystem::path& path) noexcept {
  return instance.getFile(path.begin(), path.end());
}

} /* namespace Test */
} // namespace OData
