#ifndef TEST_MOCKCONNECTION_H_
#define TEST_MOCKCONNECTION_H_

#include "Connection.h"
#include <atomic>
#include <vector>

namespace OData {
namespace Test {

class MockConnection : public Connection {
public:
  explicit MockConnection(std::uint32_t product_count);
  virtual ~MockConnection() = default;
  MockConnection(const MockConnection&) = delete;
  MockConnection& operator=(const MockConnection&) = delete;

  std::vector<std::shared_ptr<Product>> listProducts(
      SearchQuery query, std::uint32_t offset, std::uint32_t count) override;
  virtual std::vector<char> getFile(const ProductPath& path) override;
  std::shared_ptr<TemporaryFile> getTemporaryFile(
      const ProductPath& path, boost::filesystem::path tmp_file) override;
  std::vector<std::string> getDeletedProducts(std::uint32_t offset) override;
  std::unique_ptr<Connection> clone() const noexcept override;

  std::uint32_t getProductsListed() const noexcept;
  void addDeletedProduct(std::uint32_t id) noexcept;

private:
  std::uint32_t product_count;
  std::atomic<std::uint32_t> products_listed;
  std::vector<std::uint32_t> deleted_products;
};

} /* namespace Test */
} /* namespace OData */

#endif /* TEST_MOCKCONNECTION_H_ */
