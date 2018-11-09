#ifndef SRC_CACHEDSTORAGE_H_
#define SRC_CACHEDSTORAGE_H_

#include "LRUCache.h"
#include "ProductStorage.h"
#include <memory>

namespace OData {

class CachedStorage : public ProductStorage {
public:
  explicit CachedStorage(std::unique_ptr<ProductStorage> storage);
  virtual ~CachedStorage() = default;
  CachedStorage(const CachedStorage&) = delete;
  CachedStorage& operator=(const CachedStorage&) = delete;

  void storeProduct(std::shared_ptr<Product> product) override;
  bool productExists(const std::string& product_id) noexcept override;
  std::shared_ptr<Product> getProduct(const std::string& product_id) override;
  void deleteProduct(const std::string& product_id) override;
  std::unique_ptr<ProductIterator> iterator() override;

private:
  std::unique_ptr<ProductStorage> storage;
  LRUCache<std::string, std::shared_ptr<Product>> cache;
};

} /* namespace OData */

#endif /* SRC_CACHEDSTORAGE_H_ */
