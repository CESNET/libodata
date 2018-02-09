#ifndef SRC_SEARCHQUERYBUILDER_H_
#define SRC_SEARCHQUERYBUILDER_H_

#include "SearchQuery.h"
#include <cstdint>
#include <string>

namespace OData {

class SearchQueryBuilder {
public:
  SearchQueryBuilder() noexcept;
  ~SearchQueryBuilder();
  SearchQueryBuilder(const SearchQueryBuilder&) = delete;
  SearchQueryBuilder& operator=(const SearchQueryBuilder&) = delete;

  void setQuery(SearchQuery query) noexcept;
  void setStart(unsigned start) noexcept;
  void setRows(unsigned rows);
  void setOrder(std::string attribute, bool ascending) noexcept;
  std::string build() const noexcept;

private:
  SearchQuery query;
  unsigned start;
  unsigned rows;
  std::string order_by;
  bool ascending;
};

} /* namespace OData */

#endif /* SRC_SEARCHQUERYBUILDER_H_ */
