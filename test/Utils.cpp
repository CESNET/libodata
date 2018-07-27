#include "Utils.h"

#include "Directory.h"
#include "File.h"
#include "FileSystemNode.h"
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
  std::unique_ptr<Product> product(new Product(
      std::move(id),
      "name",
      "date",
      "filename",
      std::move(platform),
      "type",
      1000UL));

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
  std::unique_ptr<Directory> filesystem(new Directory("root"));
  filesystem->appendProducts(std::move(products));
  return filesystem;
}

std::shared_ptr<FileSystemNode> testGetFile(
    const FileSystemNode& instance,
    const boost::filesystem::path& path) noexcept {
  return instance.getFile(path.begin(), path.end());
}

} /* namespace Test */
} /* namespace OData */
