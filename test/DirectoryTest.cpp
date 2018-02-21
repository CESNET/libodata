#include "../src/Directory.h"

#include <gtest/gtest.h>
#include <map>
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
  std::map<std::string, std::unique_ptr<FileSystemNode>> sub_dirs;
  sub_dirs["empty_sub_dir"] = std::unique_ptr<Directory>(
      new Directory("empty_sub_dir", std::vector<std::string>{}));
  sub_dirs["sub_dir1"] = std::unique_ptr<Directory>(
      new Directory("sub_dir1", std::vector<std::string>{".manifest.xml"}));
  ASSERT_EQ(Directory("directories", {}, std::move(sub_dirs)), *directories);
} // namespace Test

} // namespace Test
} // namespace OData
