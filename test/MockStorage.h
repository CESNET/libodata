#ifndef TEST_MOCKSTORAGE_H_
#define TEST_MOCKSTORAGE_H_

#include "Product.h"
#include "ProductStorage.h"
#include <map>
#include <memory>
#include <string>

namespace OData {
namespace Test {

class MockStorage : public ProductStorage {
public:
  MockStorage();
  virtual ~MockStorage() = default;
  MockStorage(const MockStorage&) = delete;
  MockStorage& operator=(const MockStorage&) = delete;
  void storeProduct(std::shared_ptr<Product> product) override;
  bool productExists(const std::string& product_id) noexcept override;
  std::shared_ptr<Product> getProduct(const std::string& product_id) override;
  void deleteProduct(const std::string& product_id) override;
  std::unique_ptr<ProductIterator> iterator() override;

  void setFail(bool fail) noexcept;

private:
  void doFail();

  std::map<std::string, std::shared_ptr<Product>> products;
  bool fail;
};

} // namespace Test
} /* namespace OData */

#endif /* TEST_MOCKSTORAGE_H_ */
