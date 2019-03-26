#ifndef SRC_PRODUCTSTORAGE_H_
#define SRC_PRODUCTSTORAGE_H_

#include <memory>
#include <string>

namespace OData {

class Product;

/**
 * Iterator over products stored in storage
 */
class ProductIterator {
public:
  ProductIterator() = default;
  virtual ~ProductIterator() = default;
  ProductIterator(const ProductIterator&) = delete;
  ProductIterator& operator=(const ProductIterator&) = delete;

  /**
   * @return next product or null if last product was reached
   * @exception if storage error occurs
   */
  virtual std::shared_ptr<Product> next() = 0;
};

/**
 * Product storage interface. All operations must be thread safe.
 */
class ProductStorage {
public:
  ProductStorage() = default;
  virtual ~ProductStorage() = default;
  ProductStorage(const ProductStorage&) = delete;
  ProductStorage& operator=(const ProductStorage&) = delete;

  /**
   * Store product into storage
   * @param product to store
   * @exception OData::DataHubException if operation fails
   */
  virtual void storeProduct(std::shared_ptr<Product> product) = 0;
  /**
   * Check if product exists
   * @param product_id
   * @return if product is stored in storage
   */
  virtual bool productExists(const std::string& product_id) noexcept = 0;
  /**
   * Load product from storage
   * @param product_id
   * @exception OData::DataHubException if operation fails or product does not
   * exist
   */
  virtual std::shared_ptr<Product> getProduct(
      const std::string& product_id) = 0;
  /**
   * Delete product from storage
   * @param product_id
   * @exception OData::DataHubException if operation fails
   */
  virtual void deleteProduct(const std::string& product_id) = 0;
  /**
   * @return iterator over all stored records
   */
  virtual std::unique_ptr<ProductIterator> iterator() = 0;
};

} /* namespace OData */

#endif /* SRC_PRODUCTSTORAGE_H_ */
