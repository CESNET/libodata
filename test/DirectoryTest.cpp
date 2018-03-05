#include "Directory.h"

#include "ProductPath.h"
#include "RemoteFile.h"
#include <boost/filesystem/path.hpp>
#include <gtest/gtest.h>
#include <list>
#include <map>
#include <memory>

namespace OData {
namespace Test {
namespace {
const ProductPath test_path("uuid", "filename");
std::unique_ptr<Directory> createTestTree() {
  return Directory::createRemoteStructure(
      test_path,
      "test_tree",
      {"sub_dir1/.manifest.xml", "sub_dir1/sub_dir2/xyz"});
}
} // namespace

TEST(DirectoryTest, CreateDirectoryTest) {
  {
    const auto empty = Directory::createRemoteStructure(test_path, "empty", {});
    ASSERT_EQ(Directory("empty", {}), *empty);
  }

  {
    const auto only_files = Directory::createRemoteStructure(
        test_path, "files", {"manifest.xml", "file1.xml"});
    Directory::Content test_content;
    test_content["manifest.xml"] =
        std::unique_ptr<RemoteFile>(new RemoteFile("manifest.xml", test_path));
    test_content["file1.xml"] =
        std::unique_ptr<RemoteFile>(new RemoteFile("file1.xml", test_path));
    ASSERT_EQ(Directory("files", std::move(test_content)), *only_files);
  }

  {
    const auto directories = Directory::createRemoteStructure(
        test_path, "directories", {"sub_dir1/.manifest.xml", "empty_sub_dir/"});
    std::map<std::string, std::unique_ptr<FileSystemNode>> sub_dirs;
    sub_dirs["empty_sub_dir"] =
        std::unique_ptr<Directory>(new Directory("empty_sub_dir"));
    Directory::Content test_content;
    test_content[".manifest.xml"] = std::unique_ptr<RemoteFile>(
        new RemoteFile(".manifest.xml", ProductPath(test_path, "sub_dir1")));
    sub_dirs["sub_dir1"] = std::unique_ptr<Directory>(
        new Directory("sub_dir1", std::move(test_content)));
    ASSERT_EQ(Directory("directories", std::move(sub_dirs)), *directories);
  }
}

TEST(DirectoryTest, TraverseTest) {
  {
    const boost::filesystem::path path("x/y/z");
    const auto empty = Directory::createRemoteStructure(test_path, "empty", {});
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
    const boost::filesystem::path manifest("test_tree/sub_dir1/.manifest.xml");
    ASSERT_EQ(
        RemoteFile(".manifest.xml", ProductPath(test_path, "sub_dir1")),
        *test_tree->getFile(manifest.begin(), manifest.end()));
    const boost::filesystem::path subdir2("test_tree/sub_dir1/sub_dir2");
    std::map<std::string, std::unique_ptr<FileSystemNode>> sub_dirs;
    sub_dirs["xyz"] = std::unique_ptr<FileSystemNode>(new RemoteFile(
        "xyz", ProductPath("uuid", "filename", "sub_dir1/sub_dir2")));
    ASSERT_EQ(
        Directory("sub_dir2", std::move(sub_dirs)),
        *test_tree->getFile(subdir2.begin(), subdir2.end()));
  }
}

TEST(DirectoryTest, ReaddirTest) {
  {
    const auto empty = Directory::createRemoteStructure(test_path, "empty", {});
    ASSERT_EQ(std::vector<std::string>{}, empty->readDir());
  }
  {
    const auto test_tree = createTestTree();
    ASSERT_EQ(std::vector<std::string>{"sub_dir1"}, test_tree->readDir());
    std::vector<std::string> sub_dir1_expected{".manifest.xml", "sub_dir2"};
    ASSERT_EQ(sub_dir1_expected, test_tree->getChild("sub_dir1")->readDir());
  }
}

} // namespace Test
} // namespace OData
