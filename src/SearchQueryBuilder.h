#ifndef SRC_SEARCHQUERYBUILDER_H_
#define SRC_SEARCHQUERYBUILDER_H_

#include <cstdint>
#include <string>

namespace OData {

class SearchQuery {
public:
  enum class Operator { AND, OR, NOT };
  SearchQuery() = default;
  SearchQuery(const std::string& keyword, const std::string& value) noexcept;
  SearchQuery(const std::string& keyword, std::uint32_t value) noexcept;
  SearchQuery(SearchQuery left, SearchQuery right, Operator op) noexcept;
  SearchQuery(const SearchQuery&) = delete;
  SearchQuery(SearchQuery&&) = default;
  SearchQuery& operator=(const SearchQuery&) = delete;
  SearchQuery& operator=(SearchQuery&&) = default;

  const std::string& getQuery() const noexcept;

private:
  std::string query;
};

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
