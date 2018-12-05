#include "SearchQuery.h"

#include <assert.h>

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
    // assert that all enum values are implemented
    assert("Unknown enum value!");
    return nullptr;
  }
}
} // namespace

SearchQuery::SearchQuery(const char* keyword, const std::string& value) noexcept
    : query(std::string(keyword) + ":" + value) {
}

SearchQuery::SearchQuery(const char* keyword, std::uint32_t value) noexcept
    : SearchQuery(keyword, std::to_string(value)) {
}

SearchQuery::SearchQuery(
    SearchQuery left, SearchQuery right, Operator op) noexcept
    : query(
          "(" + left.query + "%20" + getOperatorString(op) + "%20" + right.query
          + ")") {
}

const std::string& SearchQuery::getQuery() const noexcept {
  return query;
}

} // namespace OData
