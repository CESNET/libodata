#ifndef SRC_SEARCHQUERY_H_
#define SRC_SEARCHQUERY_H_

#include <string>

namespace OData {

class SearchQuery {
public:
  enum class Operator { AND, OR, NOT };
  enum class Keyword {
    PLATFORM,
    BEGIN_POSITION,
    END_POSITION,
    INGESTION_DATE,
    COLLECTION,
    FILENAME,
    FOOTPRINT,
    ORBIT_NUMBER,
    LAST_ORBIT_NUMBER,
    RELATIVE_ORBIT_NUMBER,
    LAST_RELATIVE_ORBIT_NUMBER,
    ORBIT_DIRECTION,
    POLARISATION_MODE,
    PRODUCT_TYPE,
    SENSOR_OPER_MODE,
    SWATH_ID,
    CLOUD_COVERAGE,
  };
  SearchQuery() = default;
  SearchQuery(Keyword keyword, const std::string& value) noexcept;
  SearchQuery(Keyword keyword, std::uint32_t value) noexcept;
  SearchQuery(SearchQuery left, SearchQuery right, Operator op) noexcept;
  SearchQuery(const SearchQuery&) = default;
  SearchQuery(SearchQuery&&) = default;
  SearchQuery& operator=(const SearchQuery&) = default;
  SearchQuery& operator=(SearchQuery&&) = default;

  const std::string& getQuery() const noexcept;

private:
  std::string query;
};

} // namespace OData

#endif /* SRC_SEARCHQUERY_H_ */
