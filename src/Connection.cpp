#include "Connection.h"

#include "Product.h"
#include "SearchQueryBuilder.h"
#include "XmlParser.h"
#include <curl/curl.h>
#include <functional>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdexcept>

// TODO better exceptions

namespace OData {
struct ScopeGuard {
  ScopeGuard(std::function<void()> cleanup) : cleanup(std::move(cleanup)) {
  }
  ~ScopeGuard() {
    cleanup();
  }
  std::function<void()> cleanup;
};

struct Connection::Impl {
  Impl(
      std::string url, const std::string& username, const std::string& password)
      : curl_handle(curl_easy_init(), curl_easy_cleanup),
        url(std::move(url)),
        auth_token(username + ":" + password),
        body(),
        response_parser() {
  }

  ~Impl() = default;

  std::vector<char> getQuery(const std::string& uri) {
    std::cout << "Sending query: " << uri << std::endl;
    ScopeGuard guard([&]() {
      curl_easy_reset(curl_handle.get());
      body.clear();
    });

    const auto url = this->url + uri;
    curl_easy_setopt(curl_handle.get(), CURLOPT_URL, url.c_str());
    curl_easy_setopt(
        curl_handle.get(), CURLOPT_WRITEFUNCTION, Impl::dataCallback);
    curl_easy_setopt(curl_handle.get(), CURLOPT_WRITEDATA, this);
    curl_easy_setopt(curl_handle.get(), CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    curl_easy_setopt(curl_handle.get(), CURLOPT_USERPWD, auth_token.c_str());
    curl_easy_setopt(curl_handle.get(), CURLOPT_FOLLOWLOCATION, 1L);
    const auto ret = curl_easy_perform(curl_handle.get());
    if (ret == CURLE_OK) {
      int64_t http_code = 0;
      curl_easy_getinfo(curl_handle.get(), CURLINFO_RESPONSE_CODE, &http_code);
      if (http_code == 200) {
        return std::move(body);
      } else {
        body.push_back('\0');
        throw std::runtime_error(std::string(body.data()));
      }
    } else {
      throw std::runtime_error(curl_easy_strerror(ret));
    }
  }

  std::vector<char> sendListQuery(SearchQuery query, std::uint32_t offset) {
    SearchQueryBuilder query_builder;
    query_builder.setQuery(std::move(query));
    query_builder.setStart(offset);
    query_builder.setRows(100);
    query_builder.setOrder("ingestiondate", true);
    return getQuery(query_builder.build());
  }

  static size_t dataCallback(
      void* data, size_t size, size_t nmemb, void* user_data) {
    auto self = reinterpret_cast<Connection::Impl*>(user_data);
    const auto data_length = size * nmemb;
    self->body.reserve(self->body.size() + data_length);
    const char* ch_data = reinterpret_cast<char*>(data);
    std::copy(ch_data, ch_data + data_length, std::back_inserter(self->body));
    return data_length;
  }

  std::unique_ptr<CURL, std::function<void(CURL*)>> curl_handle;
  std::string url;
  std::string auth_token;
  std::vector<char> body;
  XmlParser response_parser;
};

Connection::Connection(
    std::string url, const std::string& username, const std::string& password)
    : pimpl(std::make_unique<Impl>(std::move(url), username, password)) {
}

Connection::~Connection() = default;

std::vector<std::unique_ptr<Product>> Connection::listProducts(
    SearchQuery query, uint32_t count) {
  std::vector<std::unique_ptr<Product>> products;
  while (products.size() < count) {
    auto list = pimpl->response_parser.parseList(
        pimpl->sendListQuery(query, products.size()));
    if (list.empty()) {
      break;
    } else {
      std::move(list.begin(), list.end(), std::back_inserter(products));
    }
  }
  return products;
}

void Connection::updateProductFileStructure(Product& product) {
  auto manifest_path = product.getProductPath();
  manifest_path.appendPath({product.getManifestFilename()});
  auto response = getFile(manifest_path);
  product.setArchiveStructure(Directory::create(
      product.getFilename(), pimpl->response_parser.parseManifest(response)));
}

std::vector<char> Connection::getFile(const ProductPath& path) {
  return pimpl->getQuery("odata/v1/" + path.getPath());
}

} /* namespace OData */
