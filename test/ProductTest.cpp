#include "Product.h"

#include "Directory.h"
#include "File.h"
#include "RemoteFile.h"
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
  Product expected("id", "name", "date", "file", "platform", "type");
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

} // namespace Test
} // namespace OData
