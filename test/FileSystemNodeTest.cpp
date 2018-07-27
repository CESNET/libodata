#include "FileSystemNode.h"

#include "Directory.h"
#include "File.h"
#include "Product.h"
#include "RemoteFile.h"
#include "Utils.h"
#include <boost/filesystem/path.hpp>
#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <vector>

namespace OData {
namespace Test {

TEST(FileSystemNodeTest, FileTreeTraverseTest) {
  std::vector<std::shared_ptr<Product>> products{
      createProduct("id", "platform1"), createProduct("id", "platform2")};
  const std::unique_ptr<FileSystemNode> test_tree =
      createFilesystem(std::move(products));
  ASSERT_EQ(nullptr, testGetFile(*test_tree, "abc/def").get());
  ASSERT_EQ(
      RemoteFile(
          "manifest.xml", ProductPath("uuid", "filename", "manifest.xml"), 10),
      *testGetFile(*test_tree, "platform1/date/name/extracted/manifest.xml"));
  ASSERT_EQ(
      *Directory::createRemoteStructure(
          ProductPath("uuid", "filename", "subdir"), "subdir", {{"xxx", 20}}),
      *testGetFile(*test_tree, "platform1/date/name/extracted/subdir"));
  ASSERT_EQ(
      *createProduct("id", "platform1"),
      *testGetFile(*test_tree, "platform1/date/name"));
}

} // namespace Test
} // namespace OData
