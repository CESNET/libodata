#include "SearchQueryBuilder.h"

#include <sstream>
#include <stdexcept>

namespace OData {

SearchQueryBuilder::SearchQueryBuilder() noexcept
    : query(), offset(0), result_count(10), order_by("ingestiondate"), ascending(false) {
}

SearchQueryBuilder::~SearchQueryBuilder() = default;

void SearchQueryBuilder::setQuery(SearchQuery query) noexcept {
  this->query = std::move(query);
}

void SearchQueryBuilder::setOffset(unsigned offset) noexcept {
  this->offset = offset;
}

void SearchQueryBuilder::setResultCount(unsigned result_count) {
  if (result_count > 100) {
    throw std::out_of_range("Number of rows must be in interval <0, 100>");
  }
  this->result_count = result_count;
}

void SearchQueryBuilder::setOrder(
    std::string attribute, bool ascending) noexcept {
  this->order_by = attribute;
  this->ascending = ascending;
}

std::string SearchQueryBuilder::build() const noexcept {
  std::stringstream str;
  str << "search?start=" << offset << "&rows=" << result_count << "&orderby=" << order_by
      << "%20" << (ascending ? "asc" : "desc") << "&q=" << query.getQuery();
  return str.str();
}

} /* namespace OData */
