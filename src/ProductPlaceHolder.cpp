#include "ProductPlaceHolder.h"

#include "Product.h"
#include "ProductStorage.h"
#include <exception>
#include <glog/logging.h>

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
    std::ostream& ostr, unsigned indent_level) const {
  product_storage->getProduct(id)->toString(ostr, indent_level);
}

bool ProductPlaceHolder::compare(const FileSystemNode& node) const noexcept {
  const auto* entry = dynamic_cast<const ProductPlaceHolder*>(&node);
  if (entry != nullptr) {
    return entry->id == id && entry->name == name;
  }
  const auto* product = dynamic_cast<const Product*>(&node);
  const auto instance = getInstance();
  if (product != nullptr && instance != nullptr) {
    return product->compare(*instance);
  }
  return false;
}

std::string ProductPlaceHolder::getName() const noexcept {
  return name;
}

std::shared_ptr<FileSystemNode> ProductPlaceHolder::getFile(
    boost::filesystem::path::const_iterator begin,
    boost::filesystem::path::const_iterator end) const noexcept {
  auto instance = getInstance();
  if (instance != nullptr) {
    return instance->getFile(std::move(begin), std::move(end));
  } else {
    return {};
  }
}

std::vector<std::string> ProductPlaceHolder::readDir() const noexcept {
  auto instance = getInstance();
  if (instance != nullptr) {
    return instance->readDir();
  } else {
    return {};
  }
}

bool ProductPlaceHolder::isDirectory() const noexcept {
  return true;
}

std::size_t ProductPlaceHolder::getSize() const noexcept {
  auto instance = getInstance();
  if (instance != nullptr) {
    return instance->getSize();
  } else {
    return 0;
  }
}

std::shared_ptr<Product> ProductPlaceHolder::getInstance() const noexcept {
  try {
    return product_storage->getProduct(id);
  } catch (std::exception& ex) {
    LOG(ERROR) << "ProductPlaceHolder: Product '" << id
               << "' not found: " << ex.what();
    return {};
  }
}

std::shared_ptr<FileSystemNode> ProductPlaceHolder::getChild(
    const std::string& name) const noexcept {
  auto instance = getInstance();
  if (instance != nullptr) {
    return instance->getChild(name);
  }
  return {};
}

void ProductPlaceHolder::removeChild(const std::string& name) noexcept {
  auto instance = getInstance();
  if (instance != nullptr) {
    instance->removeChild(name);
  }
}

void ProductPlaceHolder::addChild(
    std::shared_ptr<FileSystemNode> child) noexcept {
  auto instance = getInstance();
  if (instance != nullptr) {
    instance->addChild(std::move(child));
  }
}

} /* namespace OData */
