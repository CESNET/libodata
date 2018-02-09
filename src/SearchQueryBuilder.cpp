#include "SearchQueryBuilder.h"

#include <sstream>
#include <stdexcept>

namespace OData {

SearchQueryBuilder::SearchQueryBuilder() noexcept
    : query(), start(0), rows(10), order_by("ingestiondate"), ascending(false) {
}

SearchQueryBuilder::~SearchQueryBuilder() = default;

void SearchQueryBuilder::setQuery(SearchQuery query) noexcept {
  this->query = std::move(query);
}

void SearchQueryBuilder::setStart(unsigned start) noexcept {
  this->start = start;
}

void SearchQueryBuilder::setRows(unsigned rows) {
  if (rows > 100) {
    throw std::out_of_range("Number of rows must be in interval <0, 100>");
  }
  this->rows = rows;
}

void SearchQueryBuilder::setOrder(
    std::string attribute, bool ascending) noexcept {
  this->order_by = attribute;
  this->ascending = ascending;
}

std::string SearchQueryBuilder::build() const noexcept {
  std::stringstream str;
  str << "search?start=" << start << "&rows=" << rows << "&orderby=" << order_by
      << "%20" << (ascending ? "asc" : "desc") << "&q=" << query.getQuery();
  return str.str();
}

} /* namespace OData */
