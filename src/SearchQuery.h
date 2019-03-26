#ifndef SRC_SEARCHQUERY_H_
#define SRC_SEARCHQUERY_H_

#include <string>

namespace OData {

/**
 * Sorl protocol search query.
 */
class SearchQuery {
public:
  /**
   * Relational operators supported by Copernicus Open Access Hub.
   * NOT might be used as unary operator and also binary with meaning:
   * X NOT Y == X AND NOT Y
   */
  enum class Operator { AND, OR, NOT };

  SearchQuery() = default;
  SearchQuery(const char* keyword, const std::string& value) noexcept;
  SearchQuery(const char* keyword, std::uint32_t value) noexcept;
  SearchQuery(SearchQuery left, SearchQuery right, Operator op) noexcept;
  SearchQuery(const SearchQuery&) = default;
  SearchQuery(SearchQuery&&) = default;
  SearchQuery& operator=(const SearchQuery&) = default;
  SearchQuery& operator=(SearchQuery&&) = default;

  /**
   * @return sorl query encoded for usage in URI
   */
  const std::string& getQuery() const noexcept;

private:
  std::string query;
};

} // namespace OData

#endif /* SRC_SEARCHQUERY_H_ */
