#include "DataHubConnection.h"

#include "DataHubException.h"
#include "Product.h"
#include "ScopeGuard.h"
#include "SearchQueryBuilder.h"
#include "XmlParser.h"
#include <algorithm>
#include <curl/curl.h>
#include <glog/logging.h>
#include <iterator>
#include <sstream>

namespace OData {
struct DataHubConnection::Impl {
  Impl(std::string url, std::string auth_token)
      : curl_handle(curl_easy_init(), curl_easy_cleanup),
        url(std::move(url)),
        auth_token(std::move(auth_token)),
        body(),
        response_parser() {
    if (this->url.back() != '/') {
      this->url.push_back('/');
    }
  }

  ~Impl() = default;

  std::vector<char> getQuery(const std::string& uri) {
    LOG(INFO) << "Sending query: " << uri;
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
    // TODO remove workaround for testing environment, add configuration option
    // for loading certificates from custom location
    curl_easy_setopt(curl_handle.get(), CURLOPT_SSL_VERIFYPEER, 0L);
    const auto ret = curl_easy_perform(curl_handle.get());
    if (ret == CURLE_OK) {
      int64_t http_code = 0;
      curl_easy_getinfo(curl_handle.get(), CURLINFO_RESPONSE_CODE, &http_code);
      if (http_code == 200) {
        return std::move(body);
      } else {
        body.push_back('\0');
        throw DataHubException("Get request " + uri, std::string(body.data()));
      }
    } else {
      throw DataHubException("Get request " + uri, curl_easy_strerror(ret));
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
    auto self = reinterpret_cast<DataHubConnection::Impl*>(user_data);
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

DataHubConnection::DataHubConnection(
    std::string url, const std::string& username, const std::string& password)
    : pimpl(new Impl(std::move(url), username + ":" + password)) {
}

DataHubConnection::DataHubConnection(std::string url, std::string auth_token)
    : pimpl(new Impl(std::move(url), std::move(auth_token))) {
}

DataHubConnection::~DataHubConnection() = default;

std::vector<std::shared_ptr<Product>> DataHubConnection::listProducts(
    SearchQuery query, std::uint32_t offset, std::uint32_t count) {
  std::vector<std::shared_ptr<Product>> products;
  while (products.size() < count) {
    auto list = pimpl->response_parser.parseList(
        pimpl->sendListQuery(query, offset + products.size()));
    if (list.empty()) {
      break;
    } else {
      std::move(list.begin(), list.end(), std::back_inserter(products));
    }
  }
  LOG(INFO) << products.size() << " product downloaded";
  return products;
}

std::vector<char> DataHubConnection::getFile(const ProductPath& path) {
  return pimpl->getQuery("odata/v1/" + path.getPath());
}

std::unique_ptr<Connection> DataHubConnection::clone() const noexcept {
  return std::unique_ptr<Connection>(
      new DataHubConnection(pimpl->url, pimpl->auth_token));
}

} /* namespace OData */
