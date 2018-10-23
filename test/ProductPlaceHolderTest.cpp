#include "ProductPlaceHolder.h"

#include "DataHubException.h"
#include "MockStorage.h"
#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include <vector>

namespace OData {
namespace Test {

TEST(ProductPlaceHolderTest, StorageErrorsTest) {
  auto storage = std::shared_ptr<MockStorage>(new MockStorage());
  ProductPlaceHolder instance("id", "name", storage);

  storage->setFail(true);

  boost::filesystem::path path("/");
  ASSERT_EQ(nullptr, instance.getFile(path.begin(), path.end()));
  ASSERT_EQ(std::vector<std::string>{}, instance.readDir());
  ASSERT_EQ(0, instance.getSize());

  std::stringstream str;
  ASSERT_THROW(instance.toString(str), DataHubException);
  ASSERT_TRUE(str.str().empty());
}

} // namespace Test
} // namespace OData
