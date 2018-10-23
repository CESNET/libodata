#include "MockStorage.h"

#include "DataHubException.h"

namespace OData {
namespace Test {
namespace {
class Iterator : public ProductIterator {
public:
  Iterator(
      std::map<std::string, std::shared_ptr<Product>>::iterator begin,
      std::map<std::string, std::shared_ptr<Product>>::iterator end)
      : it(begin), end(end) {
  }

  std::shared_ptr<Product> next() override {
    if (it == end) {
      return nullptr;
    } else {
      const auto ret = it->second;
      ++it;
      return ret;
    }
  }

private:
  std::map<std::string, std::shared_ptr<Product>>::iterator it;
  const std::map<std::string, std::shared_ptr<Product>>::iterator end;
};
} // namespace

MockStorage::MockStorage() : products(), fail(false) {
}

void MockStorage::storeProduct(std::shared_ptr<Product> product) {
  doFail();
  products[product->getId()] = product;
}

bool MockStorage::productExists(const std::string& product_id) noexcept {
  return products.find(product_id) != products.end();
}

std::shared_ptr<Product> MockStorage::getProduct(
    const std::string& product_id) {
  doFail();
  if (productExists(product_id)) {
    return products[product_id];
  } else {
    return nullptr;
  }
}

void MockStorage::deleteProduct(const std::string& product_id) {
  doFail();
  const auto it = products.find(product_id);
  if (it != products.end()) {
    products.erase(it);
  }
}

std::unique_ptr<ProductIterator> MockStorage::iterator() {
  doFail();
  return std::unique_ptr<ProductIterator>(
      new Iterator(products.begin(), products.end()));
}

void MockStorage::setFail(bool fail) noexcept {
  this->fail = fail;
}

void MockStorage::doFail() {
  if (fail) {
    throw DataHubException("Test operation failed.");
  }
}

} // namespace Test
} /* namespace OData */
