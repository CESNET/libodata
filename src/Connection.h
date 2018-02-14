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

/**
 * Rest connection to copernicus open access hub.
 * Both OData and Sorl protocols are supported.
 */
class Connection {
public:
  explicit Connection(
      std::string url,
      const std::string& username,
      const std::string& password);
  ~Connection();

  /**
   * List products specified by query. Sorl protocol is used.
   * Archive structure is not initialized.
   * @param query specification of products to list
   * @param count how many product should be listed
   * @return list of discovered products
   * @exception if service is not available or response is invalid
   */
  std::vector<std::unique_ptr<Product>> listProducts(
      SearchQuery query, uint32_t count);

  /**
   * Update product file structure. File structure is stored in manifest file.
   * @param[in, out] product product to update
   * @exception if service is not available or response is invalid
   */
  void updateProductFileStructure(Product& product);

  /**
   * Download file using OData protocol
   * @param path path to requested file
   * @return file content
   * @exception if service is not available or response is invalid
   */
  std::vector<char> getFile(const ProductPath& path);

private:
  struct Impl;
  std::unique_ptr<Impl> pimpl;
};

} /* namespace OData */

#endif /* SRC_CONNECTION_H_ */
