#include "ProductPlaceHolder.h"

#include "Product.h"
#include "ProductStorage.h"

namespace OData {

ProductPlaceHolder::ProductPlaceHolder(
    std::string id,
    std::string name,
    std::shared_ptr<ProductStorage> product_storage)
    : id(std::move(id)),
      name(std::move(name)),
      product_storage(std::move(product_storage)) {
}

void ProductPlaceHolder::toString(
    std::ostream& ostr, unsigned indent_level) const noexcept {
  product_storage->getProduct(id)->toString(ostr, indent_level);
}

bool ProductPlaceHolder::compare(const FileSystemNode& node) const noexcept {
  const auto* entry = dynamic_cast<const ProductPlaceHolder*>(&node);
  if (entry != nullptr) {
    return entry->id == id && entry->name == name;
  }
  const auto* product = dynamic_cast<const Product*>(&node);
  if (product != nullptr) {
    return product->compare(*product_storage->getProduct(id));
  }
  return false;
}

std::string ProductPlaceHolder::getName() const noexcept {
  return name;
}

const FileSystemNode* ProductPlaceHolder::getFile(
    boost::filesystem::path::const_iterator begin,
    boost::filesystem::path::const_iterator end) const noexcept {
  auto instance = product_storage->getProduct(id);
  return instance->getFile(std::move(begin), std::move(end));
}

std::vector<std::string> ProductPlaceHolder::readDir() const noexcept {
  auto instance = product_storage->getProduct(id);
  return instance->readDir();
}

bool ProductPlaceHolder::isDirectory() const noexcept {
  return true;
}

} /* namespace OData */
