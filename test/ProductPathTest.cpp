#include "ProductPath.h"
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <gtest/gtest.h>
#include <sstream>

namespace OData {
namespace Test {

TEST(ProductPathTest, BuildPathTest) {
  ProductPath complete_product("uuid", "file");
  ASSERT_EQ(
      "Products('uuid')/Nodes('file')/$value", complete_product.getPath());

  ProductPath manifest("uuid", "file", "manifest.xml");
  ASSERT_EQ(
      "Products('uuid')/Nodes('file')/Nodes('manifest.xml')/$value",
      manifest.getPath());

  ProductPath appended("uuid", "file");
  appended.append("x");
  appended.append("y");
  ASSERT_EQ(
      "Products('uuid')/Nodes('file')/Nodes('x')/Nodes('y')/$value",
      appended.getPath());
}

TEST(ProductPathTest, SerializeTest) {
  std::stringstream sstream(
      std::ios_base::in | std::ios_base::out | std::ios_base::binary);
  ProductPath expected("uuid", "file", "x/y");
  {
    boost::archive::binary_oarchive out(sstream);
    ProductPath path(expected);
    out& path;
  }

  ProductPath deserialized;
  {
    boost::archive::binary_iarchive in(sstream);
    in& deserialized;
  }

  ASSERT_EQ(expected, deserialized);
}

} // namespace Test
} // namespace OData
