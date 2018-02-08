#include "Connection.h"

#include "Product.h"
#include "SearchQueryBuilder.h"
#include "XmlParser.h"
#include <iterator>
#include <restclient-cpp/connection.h>
#include <sstream>
#include <stdexcept>

namespace OData {
struct Connection::Impl {
  Impl(
      const std::string& url,
      const std::string& username,
      const std::string& password)
      : connection(url) {
    connection.SetBasicAuth(username, password);
  }

  ~Impl() = default;

  std::string getQuery(const std::string& uri) {
    auto response = connection.get(uri);
    if (response.code != 200) {
      throw std::runtime_error(response.body);
    }
    return response.body;
  }

  std::string sendListQuery(const std::string& platform, std::uint32_t offset) {
    SearchQueryBuilder query;
    query.addQuery({"platformname", platform});
    query.addQuery({"start", offset});
    query.addQuery({"rows", 100});
    query.addQuery({"orderby", "ingestiondate asc"});
    return getQuery(query.build());
  }

  RestClient::Connection connection;
  XmlParser response_parser;
};

Connection::Connection(
    const std::string& url,
    const std::string& username,
    const std::string& password)
    : pimpl(std::make_unique<Impl>(url, username, password)) {
}

Connection::~Connection() = default;

std::vector<std::unique_ptr<Product>> Connection::listProducts(
    const std::string& platform, uint32_t size) {
  std::vector<std::unique_ptr<Product>> products;
  while (products.size() < size) {
    auto list = pimpl->response_parser.parseList(
        pimpl->sendListQuery(platform, products.size()));
    if (list.empty()) {
      break;
    } else {
      std::move(list.begin(), list.end(), std::back_inserter(products));
    }
  }
  return products;
}

void Connection::updateProductDetails(Product& product) {
  auto manifest_path = product.getProductPath();
  manifest_path.appendPath({product.getManifestFilename()});
  product.setArchiveStructure(Directory::create(
      product.getFilename(),
      pimpl->response_parser.parseManifest(getFile(manifest_path))));
}

std::string Connection::getFile(const ProductPath& path) {
  return pimpl->getQuery("odata/v1/" + path.getPath());
}

} /* namespace OData */
