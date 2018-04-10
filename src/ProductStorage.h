#ifndef SRC_PRODUCTSTORAGE_H_
#define SRC_PRODUCTSTORAGE_H_

#include <memory>
#include <string>

namespace OData {

class Product;

class ProductStorage {
public:
  ProductStorage() = default;
  virtual ~ProductStorage() = default;
  ProductStorage(const ProductStorage&) = delete;
  ProductStorage& operator=(const ProductStorage&) = delete;

  virtual void storeProduct(std::shared_ptr<Product> product) = 0;
  virtual bool productExists(const std::string& product_id) = 0;
  virtual std::shared_ptr<Product> getProduct(
      const std::string& product_id) = 0;
};

} /* namespace OData */

#endif /* SRC_PRODUCTSTORAGE_H_ */
