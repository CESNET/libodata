#include "../src/Directory.h"

#include <gtest/gtest.h>
#include <memory>

namespace OData {
namespace Test {

TEST(DirectoryTest, CreateDirectoryTest) {
  auto empty = Directory::create("empty", {});
  ASSERT_EQ(Directory("empty", {}, {}), *empty);

  auto only_files = Directory::create("files", {"./manifest.xml", "file1.xml"});
  ASSERT_EQ(Directory("files", {"manifest.xml", "file1.xml"}, {}), *only_files);

  auto directories = Directory::create(
      "directories", {"./sub_dir1/.manifest.xml", "./empty_sub_dir/"});
  std::vector<std::unique_ptr<FileSystemNode>> sub_dirs;
  sub_dirs.emplace_back(std::make_unique<Directory>(
      "empty_sub_dir",
      std::vector<std::string>{},
      std::vector<std::unique_ptr<FileSystemNode>>{}));
  sub_dirs.emplace_back(std::make_unique<Directory>(
      "sub_dir1",
      std::vector<std::string>{".manifest.xml"},
      std::vector<std::unique_ptr<FileSystemNode>>{}));
  ASSERT_EQ(Directory("directories", {}, std::move(sub_dirs)), *directories);
}

} // namespace Test
} // namespace OData
