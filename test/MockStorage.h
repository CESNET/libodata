#ifndef TEST_MOCKSTORAGE_H_
#define TEST_MOCKSTORAGE_H_

#include "Product.h"
#include "ProductStorage.h"
#include <map>
#include <memory>
#include <string>

namespace OData {

class MockStorage : public ProductStorage {
public:
  MockStorage() = default;
  virtual ~MockStorage() = default;
  MockStorage(const MockStorage&) = delete;
  MockStorage& operator=(const MockStorage&) = delete;
  void storeProduct(std::shared_ptr<Product> product) override;
  bool productExists(const std::string& product_id) override;
  std::shared_ptr<Product> getProduct(const std::string& product_id) override;

private:
  std::map<std::string, std::shared_ptr<Product>> products;
};

} /* namespace OData */

#endif /* TEST_MOCKSTORAGE_H_ */
