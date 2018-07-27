#include "CachedStorage.h"

#include "CachedStorage.h"
#include "MockStorage.h"
#include "Product.h"
#include "Utils.h"
#include <gtest/gtest.h>

namespace OData {
namespace Test {

TEST(CachedStorageTest, CRUDTest) {
  const std::shared_ptr<Product> test_product = createProduct("id", "platform");
  CachedStorage instance(std::unique_ptr<ProductStorage>(new MockStorage()));
  ASSERT_EQ(false, instance.productExists(test_product->getId()));
  instance.storeProduct(test_product);
  ASSERT_EQ(true, instance.productExists(test_product->getId()));
  const auto retrieved = instance.getProduct(test_product->getId());
  ASSERT_EQ(*test_product, *retrieved);
  instance.deleteProduct(test_product->getId());
  ASSERT_FALSE(instance.productExists(test_product->getId()));
}

} // namespace Test
} // namespace OData
