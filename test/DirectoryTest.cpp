#include "Directory.h"

#include "File.h"
#include <gtest/gtest.h>
#include <list>
#include <map>
#include <memory>

namespace OData {
namespace Test {
namespace {
std::unique_ptr<Directory> createTestTree() {
  return Directory::create(
      "test_tree", {"./sub_dir1/.manifest.xml", "./sub_dir1/sub_dir2/xyz"});
}
} // namespace

TEST(DirectoryTest, CreateDirectoryTest) {
  {
    const auto empty = Directory::create("empty", {});
    ASSERT_EQ(Directory("empty", {}), *empty);
  }

  {
    const auto only_files =
        Directory::create("files", {"./manifest.xml", "file1.xml"});
    Directory::Content test_content;
    test_content["manifest.xml"] =
        std::unique_ptr<File>(new File("manifest.xml"));
    test_content["file1.xml"] = std::unique_ptr<File>(new File("file1.xml"));
    ASSERT_EQ(Directory("files", std::move(test_content)), *only_files);
  }

  {
    const auto directories = Directory::create(
        "directories", {"./sub_dir1/.manifest.xml", "./empty_sub_dir/"});
    std::map<std::string, std::unique_ptr<FileSystemNode>> sub_dirs;
    sub_dirs["empty_sub_dir"] =
        std::unique_ptr<Directory>(new Directory("empty_sub_dir"));
    Directory::Content test_content;
    test_content[".manifest.xml"] =
        std::unique_ptr<File>(new File(".manifest.xml"));
    sub_dirs["sub_dir1"] = std::unique_ptr<Directory>(
        new Directory("sub_dir1", std::move(test_content)));
    ASSERT_EQ(Directory("directories", std::move(sub_dirs)), *directories);
  }
}

TEST(DirectoryTest, TraverseTest) {
  {
    const auto empty = Directory::create("empty", {});
    ASSERT_EQ(nullptr, empty->getFile({"x", "y", "z"}));
  }

  {
    const auto test_tree = createTestTree();
    ASSERT_EQ(nullptr, test_tree->getFile({"x", "y", "z"}));
    ASSERT_EQ(
        File(".manifest.xml"),
        *test_tree->getFile({"sub_dir1", ".manifest.xml"}));
    ASSERT_EQ(
        *Directory::create("sub_dir2", {"xyz"}),
        *test_tree->getFile({"sub_dir1", "sub_dir2"}));
  }
}

TEST(DirectoryTest, ReaddirTest) {
  {
    const auto empty = Directory::create("empty", {});
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
