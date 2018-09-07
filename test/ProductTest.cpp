#include "Product.h"

#include "Directory.h"
#include "File.h"
#include "RemoteFile.h"
#include "Utils.h"
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/filesystem/path.hpp>
#include <gtest/gtest.h>
#include <sstream>

namespace OData {
namespace Test {

TEST(ProductTest, SerializeTest) {
  std::stringstream sstream(
      std::ios_base::in | std::ios_base::out | std::ios_base::binary);
  Product expected({{"uuid", "id"},
                    {"identifier", "name"},
                    {"ingestiondate", "date"},
                    {"filename", "file"},
                    {"platformname", "platform"},
                    {"producttype", "type"},
                    {"size", "1KB"}});
  expected.setArchiveStructure(
      std::make_shared<Directory>(),
      std::make_shared<File>(
          "manifest.xml",
          std::vector<char>{
              1,
              2,
              3,
              4,
          }));
  {
    boost::archive::binary_oarchive out(sstream);
    out.register_type<Directory>();
    out.register_type<RemoteFile>();
    out.register_type<File>();
    out& expected;
  }

  Product deserialized;
  {
    boost::archive::binary_iarchive in(sstream);
    in.register_type<Directory>();
    in.register_type<RemoteFile>();
    in.register_type<File>();
    in& deserialized;
  }

  ASSERT_EQ(expected, deserialized);
}

TEST(ProductTest, AttributesTest) {
  const auto instance = createProduct("test_id", "test_platform");

  {
    const auto invalid = instance->getAttribute("invalid");
    ASSERT_FALSE(invalid.is_initialized());
  }

  {
    const auto type = instance->getAttribute("producttype");
    ASSERT_TRUE(type.is_initialized());
    ASSERT_EQ("type", type.get());
  }

  {
    const auto type = instance->getAttribute("year");
    ASSERT_TRUE(type.is_initialized());
    ASSERT_EQ("2018", type.get());
  }

  {
    const auto type = instance->getAttribute("month");
    ASSERT_TRUE(type.is_initialized());
    ASSERT_EQ("11", type.get());
  }

  {
    const auto type = instance->getAttribute("day");
    ASSERT_TRUE(type.is_initialized());
    ASSERT_EQ("16", type.get());
  }

  {
    const auto type = instance->getAttribute("date");
    ASSERT_TRUE(type.is_initialized());
    ASSERT_EQ("2018-11-16", type.get());
  }
}

} // namespace Test
} // namespace OData
