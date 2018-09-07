#include "PathBuilder.h"

#include "DataHubException.h"
#include "Product.h"
#include "Utils.h"
#include <boost/filesystem/path.hpp>
#include <gtest/gtest.h>

namespace OData {
namespace Test {

TEST(PathBuilderTest, EmptyPathTest) {
  const auto product = createProduct("id", "platform");
  PathBuilder instance("");
  ASSERT_EQ(boost::filesystem::path().string(), instance.createPath(*product).string());
}

TEST(PathBuilderTest, CreatePathTest) {
  const auto product = createProduct("id", "platform");
  PathBuilder instance(
      "/path/${platformname}/${producttype}-combination-${identifier}");
  ASSERT_EQ(
      boost::filesystem::path("/path/platform/type-combination-name").string(),
      instance.createPath(*product).string());
}

TEST(PathBuilderTest, InvalidPathTest) {
  ASSERT_THROW(PathBuilder("/$missing_bracket}"), DataHubException);
  ASSERT_THROW(PathBuilder("/${not closed"), DataHubException);
  ASSERT_THROW(PathBuilder("/${aditional bracket{}"), DataHubException);
  ASSERT_THROW(PathBuilder("/${aditional dollar$}"), DataHubException);
  ASSERT_THROW(PathBuilder("/bracket{"), DataHubException);
  ASSERT_THROW(PathBuilder("/bracket}"), DataHubException);
}

} // namespace Test

} // namespace OData