#ifndef TEST_MOCKCONNECTION_H_
#define TEST_MOCKCONNECTION_H_

#include "Connection.h"

namespace OData {
namespace Test {

class MockConnection : public Connection {
public:
  explicit MockConnection(std::uint32_t product_count);
  virtual ~MockConnection() = default;
  MockConnection(const MockConnection&) = delete;
  MockConnection& operator=(const MockConnection&) = delete;

  std::vector<std::shared_ptr<Product>> listProducts(
      SearchQuery query, std::uint32_t count) override;
  virtual std::vector<char> getFile(const ProductPath& path) override;

private:
  std::uint32_t product_count;
};

} /* namespace Test */
} /* namespace OData */

#endif /* TEST_MOCKCONNECTION_H_ */
