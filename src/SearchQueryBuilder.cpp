#include "SearchQueryBuilder.h"

#include <cassert>
#include <curl/curl.h>
#include <functional>
#include <sstream>
#include <vector>

namespace OData {
namespace {
constexpr const char* getOperatorString(SearchQuery::Operator op) {
  switch (op) {
  case SearchQuery::Operator::AND:
    return "AND";
  case SearchQuery::Operator::OR:
    return "OR";
  case SearchQuery::Operator::NOT:
    return "NOT";
  default:
    assert(false);
    return nullptr;
  }
}
} // namespace

SearchQuery::SearchQuery(
    const std::string& keyword, const std::string& value) noexcept
    : query(keyword + ":" + value) {
}

SearchQuery::SearchQuery(
    const std::string& keyword, std::uint32_t value) noexcept
    : SearchQuery(keyword, std::to_string(value)) {
}

SearchQuery::SearchQuery(
    SearchQuery left, SearchQuery right, Operator op) noexcept
    : query(
          "(" + left.query + " " + getOperatorString(op) + " " + right.query
          + ")") {
}

const std::string& SearchQuery::getQuery() const noexcept {
  return query;
}

struct SearchQueryBuilder::Impl {
  Impl() : queries(), curl(curl_easy_init(), curl_easy_cleanup) {
  }
  ~Impl() = default;

  std::string escapeString(const std::string& str) {
    auto escaped = std::unique_ptr<char, std::function<void(void*)>>(
        curl_easy_escape(curl.get(), str.c_str(), str.length()), curl_free);
    return std::string(escaped.get());
  }

  std::vector<SearchQuery> queries;
  std::unique_ptr<CURL, std::function<void(CURL*)>> curl;
};

SearchQueryBuilder::SearchQueryBuilder() noexcept
    : pimpl(std::make_unique<Impl>()) {
}

SearchQueryBuilder::~SearchQueryBuilder() = default;

void SearchQueryBuilder::addQuery(SearchQuery query) noexcept {
  pimpl->queries.push_back(std::move(query));
}

std::string SearchQueryBuilder::build() const noexcept {
  std::stringstream str;
  str << "search?q=";
  std::string separator = "";
  for (const auto& query : pimpl->queries) {
    str << separator << pimpl->escapeString(query.getQuery());
    separator = "&";
  }
  return str.str();
}

} /* namespace OData */
