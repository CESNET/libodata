#ifndef ODATA_CONNECTION_H_
#define ODATA_CONNECTION_H_

#include <cstdint>
#include <memory>
#include <vector>

namespace OData {

class Product;
class ProductPath;
class SearchQuery;
class TemporaryFile;

class Connection {
public:
  Connection() = default;
  virtual ~Connection() = default;
  Connection(const Connection&) = delete;
  Connection& operator=(const Connection&) = delete;

  /**
   * List products specified by query.
   * Archive structure is not initialized.
   * @param query specification of products to list
   * @param offset index of first product to be fetched
   * @param count how many product should be listed
   * @return list of discovered products
   * @exception if service is not available or response is invalid
   */
  virtual std::vector<std::shared_ptr<Product>> listProducts(
      SearchQuery query, std::uint32_t offset, std::uint32_t count) = 0;

  /**
   * Download file into memory
   * @param path path to requested file
   * @return file content
   * @exception if service is not available or response is invalid
   */
  virtual std::vector<char> getFile(const ProductPath& path) = 0;

  /**
   * Download file into temporary file
   * @param path path to requested file
   * @return temporary file
   * @exception if service is not available or response is invalid
   */
  virtual std::shared_ptr<TemporaryFile> getTemporaryFile(
      const ProductPath& path) = 0;

  /**
   * Clone connection object
   * @return new connection object
   */
  virtual std::unique_ptr<Connection> clone() const noexcept = 0;
};

} /* namespace OData */

#endif /* ODATA_CONNECTION_H_ */
