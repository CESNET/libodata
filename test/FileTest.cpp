#include "File.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/filesystem/path.hpp>
#include <gtest/gtest.h>
#include <sstream>

namespace OData {
namespace Test {

TEST(FileTest, SerializeTest) {
  std::stringstream sstream(
      std::ios_base::in | std::ios_base::out | std::ios_base::binary);
  File expected("test_file", {1, 2, 3, 4});
  {
    boost::archive::binary_oarchive out(sstream);
    out& expected;
  }

  File deserialized;
  {
    boost::archive::binary_iarchive in(sstream);
    in& deserialized;
  }

  ASSERT_EQ(expected, deserialized);
}

} // namespace Test
} // namespace OData
