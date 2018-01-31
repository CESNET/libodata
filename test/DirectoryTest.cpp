#include "../src/Directory.h"

#include <gtest/gtest.h>

namespace OData {
namespace Test {

TEST(DirectoryTest, CreateDirectoryTest) {
  auto empty = Directory::create("empty", {});
  ASSERT_EQ(Directory("empty", {}, {}), empty);

  auto only_files = Directory::create("files", {"./manifest.xml", "file1.xml"});
  ASSERT_EQ(Directory("files", {"manifest.xml", "file1.xml"}, {}), only_files);

  auto directories = Directory::create(
      "directories", {"./sub_dir1/.manifest.xml", "./empty_sub_dir/"});
  std::vector<Directory> sub_dirs;
  sub_dirs.emplace_back(Directory("empty_sub_dir", {}, {}));
  sub_dirs.emplace_back(Directory("sub_dir1", {".manifest.xml"}, {}));
  ASSERT_EQ(Directory("directories", {}, std::move(sub_dirs)), directories);
}

} // namespace Test
} // namespace OData
