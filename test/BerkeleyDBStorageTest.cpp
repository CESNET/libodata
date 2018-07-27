#include "BerkeleyDBStorage.h"

#include "Product.h"
#include "ScopeGuard.h"
#include "Utils.h"
#include <cstdio>
#include <gtest/gtest.h>
#include <memory>

namespace OData {
namespace Test {

TEST(BerkeleyDBStorageTest, CRUDTest) {
  ScopeGuard guard([]() { std::remove("products.db"); });
  const std::shared_ptr<Product> test_product = createProduct("id", "platform");
  BerkeleyDBStorage instance("products.db");
  ASSERT_EQ(false, instance.productExists(test_product->getId()));
  instance.storeProduct(test_product);
  ASSERT_EQ(true, instance.productExists(test_product->getId()));
  const auto retrieved = instance.getProduct(test_product->getId());
  ASSERT_EQ(*test_product, *retrieved);
  instance.deleteProduct(test_product->getId());
  ASSERT_FALSE(instance.productExists(test_product->getId()));
}

TEST(BerkeleyDBStorageTest, IteratorTest) {
  ScopeGuard guard([]() { std::remove("products.db"); });
  const std::shared_ptr<Product> test_product1 =
      createProduct("id1", "platform");
  const std::shared_ptr<Product> test_product2 =
      createProduct("id2", "platform");
  const std::shared_ptr<Product> test_product3 =
      createProduct("id3", "platform");
  BerkeleyDBStorage instance("products.db");
  instance.storeProduct(test_product1);
  instance.storeProduct(test_product2);
  instance.storeProduct(test_product3);

  {
    auto iterator = instance.iterator();
    // FIXME not reliable?
    ASSERT_EQ(*test_product1, *iterator->next());
    ASSERT_EQ(*test_product3, *iterator->next());
    ASSERT_EQ(*test_product2, *iterator->next());
    ASSERT_EQ(nullptr, iterator->next().get());
  }
}

} // namespace Test
} // namespace OData
