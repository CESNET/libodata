#include "MockConnection.h"

#include "Product.h"
#include "SearchQuery.h"
#include "TemporaryFile.h"
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

class MockFile : public TemporaryFile {
public:
  MockFile(std::string data) : data(std::move(data)) {
  }
  virtual ~MockFile() = default;

  std::vector<char> read(off64_t offset, std::size_t length) noexcept override {
    try {
      const auto chunk = data.substr(offset, length);
      return std::vector<char>(chunk.begin(), chunk.end());
    } catch (...) {
      return {};
    }
  }

private:
  const std::string data;
};
} // namespace

MockConnection::MockConnection(std::uint32_t product_count)
    : product_count(product_count), products_listed(0), deleted_products() {
}

std::vector<std::shared_ptr<Product>> MockConnection::listProducts(
    SearchQuery, std::uint32_t offset, std::uint32_t count) {
  std::vector<std::shared_ptr<Product>> products;
  for (auto i = offset; i < count && product_count != 0; ++i, --product_count) {
    products.push_back(
        std::make_shared<Product>(std::map<std::string, std::string>{
            {"uuid", generateString("TEST_UUID", i)},
            {"identifier", generateString("TEST_NAME", i)},
            {"beginposition", generateTimestamp(i)},
            {"filename", generateString("TEST_FILE", i)},
            {"platformname", "TEST_PLATFORM"},
            {"size", "1000000KB"}}));
    ++products_listed;
  }
  return products;
}

std::vector<char> MockConnection::getFile(const ProductPath&) {
  return readTestInstance("testmanifest.xml");
}

std::shared_ptr<TemporaryFile> MockConnection::getTemporaryFile(
    const ProductPath& path, boost::filesystem::path) {
  return std::make_shared<MockFile>(path.getPath());
}

std::vector<std::string> MockConnection::getDeletedProducts(
    std::uint32_t offset) {
  std::vector<std::string> result;
  for (std::uint32_t i = offset; i < deleted_products.size(); ++i) {
    result.push_back(generateString("TEST_UUID", deleted_products[i]));
  }
  return result;
}

std::unique_ptr<Connection> MockConnection::clone() const noexcept {
  return std::unique_ptr<Connection>(new MockConnection(product_count));
}

std::uint32_t OData::Test::MockConnection::getProductsListed() const noexcept {
  return products_listed;
}

void MockConnection::addDeletedProduct(std::uint32_t id) noexcept {
  deleted_products.push_back(id);
}

} /* namespace Test */
} // namespace OData
