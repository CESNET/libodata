#include "Directory.h"

#include "File.h"
#include "Product.h"
#include "ProductPath.h"
#include "RemoteFile.h"
#include "Utils.h"
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/filesystem/path.hpp>
#include <gtest/gtest.h>
#include <list>
#include <map>
#include <memory>
#include <sstream>

namespace OData {
namespace Test {
namespace {
ProductPath createTestPath(std::string path = {}) {
  return ProductPath("uuid", "filename", path);
}

std::unique_ptr<Directory> createTestTree() {
  return Directory::createRemoteStructure(
      createTestPath(),
      "test_tree",
      {{"sub_dir1/.manifest.xml", 100}, {"sub_dir1/sub_dir2/xyz", 200}});
}
} // namespace

TEST(DirectoryTest, CreateDirectoryTest) {
  {
    const auto empty =
        Directory::createRemoteStructure(createTestPath(), "empty", {});
    ASSERT_EQ(Directory("empty", {}), *empty);
  }

  {
    const auto only_files = Directory::createRemoteStructure(
        createTestPath(), "files", {{"manifest.xml", 100}, {"file1.xml", 200}});
    Directory::Content test_content;
    test_content["manifest.xml"] = std::make_unique<RemoteFile>(
        "manifest.xml", createTestPath("manifest.xml"), 100);
    test_content["file1.xml"] = std::make_unique<RemoteFile>(
        "file1.xml", createTestPath("file1.xml"), 200);
    ASSERT_EQ(Directory("files", std::move(test_content)), *only_files);
  }

  {
    const auto directories = Directory::createRemoteStructure(
        createTestPath(),
        "directories",
        {{"sub_dir1/.manifest.xml", 500}, {"empty_sub_dir/", 600}});
    std::map<std::string, std::shared_ptr<FileSystemNode>> sub_dirs;
    sub_dirs["empty_sub_dir"] = std::make_shared<Directory>("empty_sub_dir");
    Directory::Content test_content;
    test_content[".manifest.xml"] = std::make_shared<RemoteFile>(
        ".manifest.xml", createTestPath("sub_dir1/.manifest.xml"), 500);
    sub_dirs["sub_dir1"] =
        std::make_shared<Directory>("sub_dir1", std::move(test_content));
    ASSERT_EQ(Directory("directories", std::move(sub_dirs)), *directories);
  }
}

TEST(DirectoryTest, TraverseTest) {
  {
    const boost::filesystem::path path("x/y/z");
    const auto empty =
        Directory::createRemoteStructure(createTestPath(), "empty", {});
    ASSERT_EQ(nullptr, empty->getFile(path.begin(), path.end()));
  }

  {
    const boost::filesystem::path empty;
    const auto test_tree = createTestTree();
    ASSERT_EQ(nullptr, test_tree->getFile(empty.begin(), empty.end()));
  }

  {
    const auto test_tree = createTestTree();
    const boost::filesystem::path invalid("test_tree/x/y/z");
    ASSERT_EQ(nullptr, test_tree->getFile(invalid.begin(), invalid.end()));
    const boost::filesystem::path manifest("sub_dir1/.manifest.xml");
    ASSERT_EQ(
        RemoteFile(
            ".manifest.xml", createTestPath("sub_dir1/.manifest.xml"), 100),
        *test_tree->getFile(manifest.begin(), manifest.end()));
    const boost::filesystem::path subdir2("sub_dir1/sub_dir2");
    std::map<std::string, std::shared_ptr<FileSystemNode>> sub_dirs;
    sub_dirs["xyz"] = std::make_shared<RemoteFile>(
        "xyz", createTestPath("sub_dir1/sub_dir2/xyz"), 200);
    ASSERT_EQ(
        Directory("sub_dir2", std::move(sub_dirs)),
        *test_tree->getFile(subdir2.begin(), subdir2.end()));
  }
}

TEST(DirectoryTest, ReaddirTest) {
  {
    const auto empty =
        Directory::createRemoteStructure(createTestPath(), "empty", {});
    ASSERT_EQ(std::vector<std::string>{}, empty->readDir());
  }
  {
    const auto test_tree = createTestTree();
    ASSERT_EQ(std::vector<std::string>{"sub_dir1"}, test_tree->readDir());
    std::vector<std::string> sub_dir1_expected{".manifest.xml", "sub_dir2"};
    ASSERT_EQ(sub_dir1_expected, test_tree->getChild("sub_dir1")->readDir());
  }
}

TEST(DirectoryTest, SerializeTest) {
  std::stringstream sstream(
      std::ios_base::in | std::ios_base::out | std::ios_base::binary);
  auto expected = createTestTree();
  {
    boost::archive::binary_oarchive out(sstream);
    out.register_type<Directory>();
    out.register_type<RemoteFile>();
    out.register_type<File>();
    out&(*expected);
  }

  Directory deserialized;
  {
    boost::archive::binary_iarchive in(sstream);
    in.register_type<Directory>();
    in.register_type<RemoteFile>();
    in.register_type<File>();
    in& deserialized;
  }

  ASSERT_EQ(*expected, deserialized);
}

} // namespace Test
} // namespace OData
