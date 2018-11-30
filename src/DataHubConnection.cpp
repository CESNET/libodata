#include "DataHubConnection.h"

#include "DataHubException.h"
#include "Product.h"
#include "ProductAttribute.h"
#include "ScopeGuard.h"
#include "SearchQueryBuilder.h"
#include "TemporaryFile.h"
#include "XmlParser.h"
#include <algorithm>
#include <curl/curl.h>
#include <fstream>
#include <glog/logging.h>
#include <iterator>
#include <sstream>

namespace OData {
struct FileDataCallback {

  FileDataCallback(const boost::filesystem::path& path)
      : file(path.c_str(), std::fstream::out | std::fstream::binary) {
  }

  ~FileDataCallback() {
    file.flush();
  }

  static size_t dataCallback(
      void* data, size_t size, size_t nmemb, void* user_data) {
    auto self = reinterpret_cast<FileDataCallback*>(user_data);
    const auto data_length = size * nmemb;
    const char* ch_data = reinterpret_cast<char*>(data);
    self->file.write(ch_data, data_length);
    return data_length;
  }

  void processError(const std::string& uri) {
    file << '\0';
    throw DataHubException("Get request " + uri, std::string());
  }

  std::fstream file;
};

struct MemoryCallback {

  static size_t dataCallback(
      void* data, size_t size, size_t nmemb, void* user_data) {
    auto self = reinterpret_cast<MemoryCallback*>(user_data);
    const auto data_length = size * nmemb;
    self->body.reserve(self->body.size() + data_length);
    const char* ch_data = reinterpret_cast<char*>(data);
    std::copy(ch_data, ch_data + data_length, std::back_inserter(self->body));
    return data_length;
  }

  void processError(const std::string& uri) {
    body.push_back('\0');
    throw DataHubException("Get request " + uri, std::string(body.data()));
  }

  std::vector<char> body;
};

struct DataHubConnection::Impl {
  Impl(std::string url, std::string auth_token, bool validate_certificate)
      : curl_handle(curl_easy_init(), curl_easy_cleanup),
        url(std::move(url)),
        auth_token(std::move(auth_token)),
        validate_certificate(validate_certificate),
        response_parser() {
    if (this->url.back() != '/') {
      this->url.push_back('/');
    }
  }

  ~Impl() = default;

  template <typename DataCallback>
  void getQuery(const std::string& uri, DataCallback* callback) {
    LOG(INFO) << "Sending query: " << uri;

    const auto url = this->url + uri;
    curl_easy_setopt(curl_handle.get(), CURLOPT_URL, url.c_str());
    curl_easy_setopt(
        curl_handle.get(), CURLOPT_WRITEFUNCTION, DataCallback::dataCallback);
    curl_easy_setopt(curl_handle.get(), CURLOPT_WRITEDATA, callback);
    curl_easy_setopt(curl_handle.get(), CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    curl_easy_setopt(curl_handle.get(), CURLOPT_USERPWD, auth_token.c_str());
    curl_easy_setopt(curl_handle.get(), CURLOPT_FOLLOWLOCATION, 1L);
    if (!validate_certificate) {
      curl_easy_setopt(curl_handle.get(), CURLOPT_SSL_VERIFYPEER, 0L);
    }
    const auto ret = curl_easy_perform(curl_handle.get());
    if (ret == CURLE_OK) {
      int64_t http_code = 0;
      curl_easy_getinfo(curl_handle.get(), CURLINFO_RESPONSE_CODE, &http_code);
      if (http_code != 200) {
        callback->processError(uri);
      }
    } else {
      throw DataHubException("Get request " + uri, curl_easy_strerror(ret));
    }
  }

  std::vector<char> sendListQuery(SearchQuery query, std::uint32_t offset) {
    SearchQueryBuilder query_builder;
    query_builder.setQuery(std::move(query));
    query_builder.setOffset(offset);
    query_builder.setResultCount(100);
    query_builder.setOrder(Attribute::INGESTION_DATE, true);
    MemoryCallback callback;
    getQuery(query_builder.build(), &callback);
    return std::move(callback.body);
  }

  std::unique_ptr<CURL, std::function<void(CURL*)>> curl_handle;
  std::string url;
  std::string auth_token;
  bool validate_certificate;
  XmlParser response_parser;
};

DataHubConnection::DataHubConnection(
    std::string url,
    const std::string& username,
    const std::string& password,
    bool validate_certificate)
    : pimpl(std::make_unique<Impl>(
          std::move(url), username + ":" + password, validate_certificate)) {
}

DataHubConnection::DataHubConnection(
    std::string url, std::string auth_token, bool validate_certificate)
    : pimpl(std::make_unique<Impl>(
          std::move(url), std::move(auth_token), validate_certificate)) {
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
  MemoryCallback callback;
  pimpl->getQuery("odata/v1/" + path.getPath(), &callback);
  return std::move(callback.body);
}

std::shared_ptr<TemporaryFile> DataHubConnection::getTemporaryFile(
    const ProductPath& path, boost::filesystem::path tmp_file) {
  FileDataCallback callback(tmp_file);
  const auto temporary_file =
      std::make_shared<TemporaryFileImpl>(path, std::move(tmp_file));
  pimpl->getQuery("odata/v1/" + path.getPath(), &callback);
  return temporary_file;
}

std::vector<std::string> DataHubConnection::getDeletedProducts(
    std::uint32_t offset) {
  MemoryCallback callback;
  pimpl->getQuery(
      "odata/v1/DeletedProducts?$skip=" + std::to_string(offset), &callback);
  return pimpl->response_parser.parseDeletedList(callback.body);
}

std::unique_ptr<Connection> DataHubConnection::clone() const noexcept {
  return std::make_unique<DataHubConnection>(
      pimpl->url, pimpl->auth_token, pimpl->validate_certificate);
}

} /* namespace OData */
