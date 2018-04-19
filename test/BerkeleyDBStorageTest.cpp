#include "BerkeleyDBStorage.h"

#include "Directory.h"
#include "File.h"
#include "Product.h"
#include "ScopeGuard.h"
#include <cstdio>
#include <gtest/gtest.h>
#include <memory>

namespace OData {
namespace Test {

namespace {
std::shared_ptr<Product> createProduct() {
  auto product(std::make_shared<Product>(
      "id", "name", "date", "filename", "platform", "type", 1000UL));

  product->setArchiveStructure(
      std::make_shared<Directory>(),
      std::make_shared<File>("manifest.xml", std::vector<char>{1, 0, 2, 3, 4}));
  return product;
}
} // namespace

TEST(BerkeleyDBStorageTest, CRUDTest) {
  ScopeGuard guard([]() { std::remove("products.db"); });
  const auto test_product = createProduct();
  BerkeleyDBStorage instance(".");
  ASSERT_EQ(false, instance.productExists(test_product->getId()));
  instance.storeProduct(test_product);
  ASSERT_EQ(true, instance.productExists(test_product->getId()));
  const auto retrieved = instance.getProduct(test_product->getId());
  ASSERT_EQ(*test_product, *retrieved);
}

} // namespace Test
} // namespace OData
