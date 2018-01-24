#include "Connection.h"

#include <iterator>
#include <restclient-cpp/connection.h>
#include <sstream>
#include <stdexcept>
#include "Product.h"
#include "XmlParser.h"

namespace OData {

struct Connection::Impl {
    Impl(
        const std::string& url,
        const std::string& username,
        const std::string& password);
    ~Impl() = default;
    std::string getQuery(const std::string& uri);
    std::string sendListQuery(const std::string& platform, std::uint32_t offset);
    std::string sendSafeManifestQuery(const std::string& product_path);
    std::string sendXfduManifestQuery(const std::string& product_path);

    RestClient::Connection connection;
    XmlParser response_parser;
};

Connection::Impl::Impl(
    const std::string& url,
    const std::string& username,
    const std::string& password) : connection(url) {
    connection.SetBasicAuth(username, password);
}

std::string Connection::Impl::getQuery(
    const std::string& uri) {
    auto response = connection.get(uri);
    if (response.code != 200) {
        throw std::runtime_error(response.body);
    }
    return response.body;
}

std::string Connection::Impl::sendListQuery(
    const std::string& platform,
    std::uint32_t offset) {
    std::stringstream query;
    query << "search?q=(platformname:" << platform << ")&start=" << offset << "&rows=2&orderby=beginposition%20asc";
    return getQuery(query.str());
}

std::string Connection::Impl::sendSafeManifestQuery(
    const std::string& product_path) {
    std::stringstream query;
    query << "odata/v1/" << product_path << "/Nodes('manifest.safe')/$value";
    return getQuery(query.str());
}

std::string Connection::Impl::sendXfduManifestQuery(
    const std::string& product_path) {
    std::stringstream query;
    query << "odata/v1/" << product_path << "/Nodes('xfdumanifest.xml')/$value";
    return getQuery(query.str());
}

Connection::Connection(
    const std::string& url,
    const std::string& username,
    const std::string& password) :
        pimpl(std::make_unique<Impl>(url, username, password)) {
}

Connection::~Connection() = default;

std::vector<Product> Connection::listProducts(
    const std::string& platform,
    uint32_t size) {
    std::vector<Product> products;
    while (products.size() < size) {
        auto list = pimpl->response_parser.parseList(pimpl->sendListQuery(platform, products.size()));
        for (auto& product: list) {
            std::string manifest;
            if(product.getPlatform() == "Sentinel-3") {
                manifest = std::move(pimpl->sendXfduManifestQuery(product.getProductPath()));
            } else {
                manifest = std::move(pimpl->sendSafeManifestQuery(product.getProductPath()));
            }
            product.setFiles(pimpl->response_parser.parseManifest(manifest));
        }
        std::move(list.begin(), list.end(), std::back_inserter(products));
    }
    return products;
}

} /* namespace OData */
