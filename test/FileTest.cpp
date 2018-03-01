#include "File.h"

#include <boost/filesystem/path.hpp>
#include <gtest/gtest.h>

namespace OData {
namespace Test {

TEST(FileTest, GetFileTest) {
  File file("test_file");

  const boost::filesystem::path empty;
  ASSERT_EQ(nullptr, file.getFile(empty.begin(), empty.end()));

  const boost::filesystem::path long_path("test_file/too/long");
  ASSERT_EQ(nullptr, file.getFile(long_path.begin(), long_path.end()));

  const boost::filesystem::path path("test_file");
  ASSERT_EQ(&file, file.getFile(path.begin(), path.end()));
}

} // namespace Test
} // namespace OData
