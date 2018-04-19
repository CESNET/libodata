#include "MockConnection.h"

#include "Product.h"
#include "SearchQuery.h"
#include "Utils.h"
#include <ctime>
#include <sstream>

namespace OData {
namespace Test {
namespace {
constexpr std::time_t test_epoch = 1520505922L;
constexpr std::time_t day = 24 * 3600;

std::string generateString(std::string prefix, std::uint32_t id) {
  std::stringstream str;
  str << prefix << id;
  return str.str();
}

std::string generateTimestamp(std::uint32_t id) {
  char timestamp[100];
  const auto since_epoch = test_epoch + id * day;
  std::strftime(
      timestamp, sizeof(timestamp), "%FT%T", std::gmtime(&since_epoch));
  return timestamp;
}
} // namespace

MockConnection::MockConnection(std::uint32_t product_count)
    : product_count(product_count) {
}

std::vector<std::shared_ptr<Product>> MockConnection::listProducts(
    SearchQuery, std::uint32_t offset, std::uint32_t count) {
  std::vector<std::shared_ptr<Product>> products;
  for (auto i = offset; i < count && product_count != 0; ++i, --product_count) {
    products.push_back(std::make_shared<Product>(
        generateString("TEST_UUID", i),
        generateString("TEST_NAME", i),
        generateTimestamp(i),
        generateString("TEST_FILE", i),
        "TEST_PLATFORM",
        "TEST_PRODUCT_TYPE",
        1000000000UL));
  }
  return products;
}

std::vector<char> MockConnection::getFile(const ProductPath&) {
  return readTestInstance("manifest.xml");
}

std::unique_ptr<Connection> MockConnection::clone() const noexcept {
  return std::unique_ptr<Connection>(new MockConnection(product_count));
}

} /* namespace Test */
} /* namespace OData */
