#include "FileSystemNode.h"

#include "Directory.h"
#include "File.h"
#include "Product.h"
#include "RemoteFile.h"
#include <boost/filesystem/path.hpp>
#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <vector>

namespace OData {
namespace Test {

namespace {
std::unique_ptr<Product> createTestProduct(std::string platform) {
  std::unique_ptr<Product> product(new Product(
      "id", "name", "date", "filename", std::move(platform), "type", 100));
  product->setArchiveStructure(
      Directory::createRemoteStructure(
          ProductPath("uuid", "filename"),
          "extracted",
          {{"manifest.xml", 10}, {"subdir/xxx", 20}, {"subdir2/yyy", 30}}),
      std::make_shared<File>(std::string("manifest.xml"), std::vector<char>{}));
  return product;
}

std::unique_ptr<Directory> createFilesystem(
    std::vector<std::shared_ptr<Product>> products) noexcept {
  std::unique_ptr<Directory> filesystem(new Directory("root"));
  filesystem->appendProducts(products);
  return filesystem;
}

} // namespace

TEST(FileSystemNodeTest, FileTreeTraverseTest) {
  std::vector<std::shared_ptr<Product>> products;
  products.emplace_back(createTestProduct("platform1"));
  products.emplace_back(createTestProduct("platform2"));
  const std::unique_ptr<FileSystemNode> test_tree =
      createFilesystem(std::move(products));
  ASSERT_EQ(nullptr, test_tree->getFile("abc/def"));
  ASSERT_EQ(
      RemoteFile("manifest.xml", ProductPath("uuid", "filename"), 10),
      *test_tree->getFile("platform1/date/name/extracted/manifest.xml"));
  ASSERT_EQ(
      *Directory::createRemoteStructure(
          ProductPath("uuid", "filename", "subdir"), "subdir", {{"xxx", 20}}),
      *test_tree->getFile("platform1/date/name/extracted/subdir"));
  ASSERT_EQ(
      *createTestProduct("platform1"),
      *test_tree->getFile("platform1/date/name"));
}

} // namespace Test
} // namespace OData
