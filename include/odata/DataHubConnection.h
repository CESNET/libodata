#ifndef ODATA_DATAHUBCONNECTION_H_
#define ODATA_DATAHUBCONNECTION_H_

#include "Connection.h"
#include <memory>
#include <string>

namespace OData {

class Product;
class ProductPath;
class SearchQuery;

/**
 * Rest connection to copernicus open access hub.
 * Both OData and Sorl protocols are supported.
 */
class DataHubConnection : public Connection {
public:
  explicit DataHubConnection(
      std::string url,
      const std::string& username,
      const std::string& password);
  virtual ~DataHubConnection();
  DataHubConnection(const DataHubConnection&) = delete;
  DataHubConnection& operator=(const DataHubConnection&) = delete;

  std::vector<std::shared_ptr<Product>> listProducts(
      SearchQuery query, std::uint32_t offset, std::uint32_t count) override;
  std::vector<char> getFile(const ProductPath& path) override;
  std::shared_ptr<TemporaryFile> getTemporaryFile(
      const ProductPath& path) override;
  std::unique_ptr<Connection> clone() const noexcept override;

private:
  DataHubConnection(std::string url, std::string auth_token);

  struct Impl;
  std::unique_ptr<Impl> pimpl;
};

} /* namespace OData */

#endif /* ODATA_DATAHUBCONNECTION_H_ */
