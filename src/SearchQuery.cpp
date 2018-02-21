#include "SearchQuery.h"

#include <assert.h>

namespace OData {
namespace {
// TODO constexpr in C++14
const char* getOperatorString(SearchQuery::Operator op) {
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

// TODO constexpr in C++14
const char* getKeywordString(SearchQuery::Keyword word) {
  switch (word) {
  case SearchQuery::Keyword::PLATFORM:
    return "platformname";
  case SearchQuery::Keyword::BEGIN_POSITION:
    return "beginposition";
  case SearchQuery::Keyword::END_POSITION:
    return "endposition";
  case SearchQuery::Keyword::INGESTION_DATE:
    return "ingestiondate";
  case SearchQuery::Keyword::COLLECTION:
    return "collection";
  case SearchQuery::Keyword::FILENAME:
    return "filename";
  case SearchQuery::Keyword::FOOTPRINT:
    return "footprint";
  case SearchQuery::Keyword::ORBIT_NUMBER:
    return "orbitnumber";
  case SearchQuery::Keyword::LAST_ORBIT_NUMBER:
    return "lastorbitnumber";
  case SearchQuery::Keyword::RELATIVE_ORBIT_NUMBER:
    return "relativeorbitnumber";
  case SearchQuery::Keyword::LAST_RELATIVE_ORBIT_NUMBER:
    return "lastrelativeorbitnumber";
  case SearchQuery::Keyword::ORBIT_DIRECTION:
    return "orbitdirection";
  case SearchQuery::Keyword::POLARISATION_MODE:
    return "polarisationmode";
  case SearchQuery::Keyword::PRODUCT_TYPE:
    return "producttype";
  case SearchQuery::Keyword::SENSOR_OPER_MODE:
    return "sensoroperationalmode";
  case SearchQuery::Keyword::SWATH_ID:
    return "swathidentifier";
  case SearchQuery::Keyword::CLOUD_COVERAGE:
    return "cloudcoverpercentage";
  default:
    // assert that all enum values are implemented
    assert("Unknown enum value!");
    return nullptr;
  }
}
} // namespace

SearchQuery::SearchQuery(Keyword keyword, const std::string& value) noexcept
    : query(std::string(getKeywordString(keyword)) + ":" + value) {
}

SearchQuery::SearchQuery(Keyword keyword, std::uint32_t value) noexcept
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
