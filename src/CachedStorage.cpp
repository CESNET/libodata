#include "CachedStorage.h"

#include "Product.h"

namespace OData {

CachedStorage::CachedStorage(std::unique_ptr<ProductStorage> storage)
    : storage(std::move(storage)), cache(1000) {
}

void CachedStorage::storeProduct(std::shared_ptr<Product> product) {
  storage->storeProduct(product);
  cache.put(product->getId(), product);
}

bool CachedStorage::productExists(const std::string& product_id) {
  if (cache.get(product_id).is_initialized()) {
    return true;
  } else {
    return storage->productExists(product_id);
  }
}

std::shared_ptr<Product> CachedStorage::getProduct(
    const std::string& product_id) {
  auto cached_product = cache.get(product_id);
  if (cached_product.is_initialized()) {
    return cached_product.value();
  } else {
    auto product = storage->getProduct(product_id);
    cache.put(product_id, product);
    return product;
  }
}

void CachedStorage::deleteProduct(const std::string& product_id) {
  cache.remove(product_id);
  storage->deleteProduct(product_id);
}

std::unique_ptr<ProductIterator> CachedStorage::iterator() {
  return storage->iterator();
}

} /* namespace OData */
