#include "ProductPath.h"
#include <gtest/gtest.h>

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

} // namespace Test
} // namespace OData
