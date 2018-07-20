#include "TemporaryFile.h"

#include "ProductPath.h"
#include <boost/filesystem.hpp>
#include <fstream>
#include <gtest/gtest.h>
#include <sstream>
#include <stdexcept>

namespace OData {
namespace Test {
namespace {
void createTestFile(const std::string& filename, const std::string& content) {
  std::fstream file(filename, std::fstream::out | std::fstream::binary);
  if (file.is_open()) {
    file << content;
    file.flush();
  } else {
    throw std::invalid_argument("Cannot create file " + filename);
  }
}
} // namespace

TEST(TemporaryFileTest, ReadTest) {
  createTestFile("test.tmp", "1234567890");

  {
    TemporaryFileImpl instance({"TESTUUID", "test.file"}, "test.tmp");

    ASSERT_EQ(
        (std::vector<char>{'1', '2', '3', '4', '5'}), instance.read(0, 5));
    ASSERT_TRUE(instance.read(20, 15).empty());

    ASSERT_EQ((std::vector<char>{'7', '8', '9', '0'}), instance.read(6, 50));
  }

  ASSERT_FALSE(boost::filesystem::exists("test.tmp"));
}

} // namespace Test
} // namespace OData
