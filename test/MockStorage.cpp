#include "MockStorage.h"

namespace OData {

void MockStorage::storeProduct(std::shared_ptr<Product> product) {
  products[product->getId()] = product;
}

bool MockStorage::productExists(const std::string& product_id) {
  return products.find(product_id) != products.end();
}

std::shared_ptr<Product> MockStorage::getProduct(
    const std::string& product_id) {
  if (productExists(product_id)) {
    return products[product_id];
  } else {
    return nullptr;
  }
}

} /* namespace OData */
