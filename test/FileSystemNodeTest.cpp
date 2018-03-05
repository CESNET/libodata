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
      "id", "name", "date", "filename", std::move(platform), "type"));
  product->setArchiveStructure(Directory::createRemoteStructure(
      ProductPath("uuid", "filename"),
      "extracted",
      {"manifest.xml", "subdir/xxx", "subdir2/yyy"}));
  return product;
}
} // namespace

TEST(FileSystemNodeTest, FileTreeTraverseTest) {
  std::vector<std::unique_ptr<Product>> products;
  products.emplace_back(createTestProduct("platform1"));
  products.emplace_back(createTestProduct("platform2"));
  const std::unique_ptr<FileSystemNode> test_tree =
      Directory::createFilesystem(std::move(products));
  ASSERT_EQ(nullptr, test_tree->getFile("/abc/def"));
  ASSERT_EQ(
      RemoteFile("manifest.xml", ProductPath("uuid", "filename")),
      *test_tree->getFile("/platform1/date/name/extracted/manifest.xml"));
  ASSERT_EQ(
      *Directory::createRemoteStructure(
          ProductPath("uuid", "filename", "subdir"), "subdir", {"xxx"}),
      *test_tree->getFile("/platform1/date/name/extracted/subdir"));
  ASSERT_EQ(
      *createTestProduct("platform1"),
      *test_tree->getFile("/platform1/date/name"));
}

} // namespace Test
} // namespace OData
