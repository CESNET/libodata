#ifndef SRC_CONNECTION_H_
#define SRC_CONNECTION_H_

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace OData {

class Product;
class ProductPath;
class SearchQuery;

class Connection {
public:
  explicit Connection(
      std::string url,
      const std::string& username,
      const std::string& password);
  ~Connection();
  std::vector<std::unique_ptr<Product>> listProducts(
      SearchQuery query, uint32_t size);
  void updateProductDetails(Product& product);
  std::vector<char> getFile(const ProductPath& path);

private:
  struct Impl;
  std::unique_ptr<Impl> pimpl;
};

} /* namespace OData */

#endif /* SRC_CONNECTION_H_ */
