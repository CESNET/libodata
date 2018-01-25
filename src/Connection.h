#ifndef SRC_CONNECTION_H_
#define SRC_CONNECTION_H_

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace OData {

class Product;

class Connection {
public:
  explicit Connection(
      const std::string& url,
      const std::string& username,
      const std::string& password);
  ~Connection();
  std::vector<Product> listProducts(const std::string& platform, uint32_t size);
  void updateProductDetails(Product& product);

private:
  struct Impl;
  std::unique_ptr<Impl> pimpl;
};

} /* namespace OData */

#endif /* SRC_CONNECTION_H_ */
