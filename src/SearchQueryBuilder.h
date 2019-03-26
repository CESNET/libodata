#ifndef SRC_SEARCHQUERYBUILDER_H_
#define SRC_SEARCHQUERYBUILDER_H_

#include "SearchQuery.h"
#include <cstdint>
#include <string>

namespace OData {

/**
 * OData protocol search query builder.
 * Query looks like this:
 *  1) Select all products defined by sorl query
 *  2) Sort them using specified attribute (ascending or descending)
 *  3) Skip all rows before start and get requested number of rows
 */
class SearchQueryBuilder {
public:
  SearchQueryBuilder() noexcept;
  ~SearchQueryBuilder();
  SearchQueryBuilder(const SearchQueryBuilder&) = delete;
  SearchQueryBuilder& operator=(const SearchQueryBuilder&) = delete;

  /**
   * Set sorl query
   */
  void setQuery(SearchQuery query) noexcept;

  /**
   * Set offset to query response list
   */
  void setOffset(unsigned offset) noexcept;

  /**
   * Requested rows count
   * @param result_count rows count, valid interval is 0-100
   */
  void setResultCount(unsigned result_count);

  /**
   * Set attributes for ordering query results
   * @param attribute product attribute
   * @param ascending sorting order
   */
  void setOrder(std::string attribute, bool ascending) noexcept;

  /**
   * Build query
   * @return query prepared to be used in url
   */
  std::string build() const noexcept;

private:
  SearchQuery query;
  unsigned offset;
  unsigned result_count;
  std::string order_by;
  bool ascending;
};

} /* namespace OData */

#endif /* SRC_SEARCHQUERYBUILDER_H_ */
