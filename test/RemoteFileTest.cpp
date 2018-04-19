#include "RemoteFile.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/filesystem/path.hpp>
#include <gtest/gtest.h>
#include <sstream>

namespace OData {
namespace Test {

TEST(RemoteFileTest, SerializeTest) {
  std::stringstream sstream(
      std::ios_base::in | std::ios_base::out | std::ios_base::binary);
  RemoteFile expected("test_file", {"uuid", "filename", "x/y/z"}, 100000);
  {
    boost::archive::binary_oarchive out(sstream);
    out& expected;
  }

  RemoteFile deserialized;
  {
    boost::archive::binary_iarchive in(sstream);
    in& deserialized;
  }

  ASSERT_EQ(expected, deserialized);
}

} // namespace Test
} // namespace OData
