#include "SearchQueryBuilder.h"
#include <gtest/gtest.h>

namespace OData {
namespace Test {

TEST(SearchQueryBuilderTest, SearchQueryBuilderTest) {
  SearchQueryBuilder empty;
  ASSERT_EQ(
      "search?start=0&rows=10&orderby=ingestiondate%20desc&q=", empty.build());

  SearchQueryBuilder attributes;
  attributes.setOrder("x", true);
  ASSERT_THROW(attributes.setRows(200), std::out_of_range);
  attributes.setRows(20);
  attributes.setStart(50);
  ASSERT_EQ("search?start=50&rows=20&orderby=x%20asc&q=", attributes.build());

  SearchQueryBuilder simple;
  simple.setQuery({SearchQuery::Keyword::PLATFORM, "y"});
  ASSERT_EQ(
      "search?start=0&rows=10&orderby=ingestiondate%20desc&q=platformname:y",
      simple.build());

  SearchQueryBuilder complex;
  complex.setQuery({{SearchQuery::Keyword::PLATFORM, "y"},
                    {SearchQuery::Keyword::COLLECTION, 42},
                    SearchQuery::Operator::AND});
  complex.setOrder("y", false);
  complex.setRows(100);
  complex.setStart(100);
  ASSERT_EQ(
      "search?start=100&rows=100&orderby=y%20desc&q=(platformname:y%20AND%"
      "20collection:42)",
      complex.build());
}

} // namespace Test
} // namespace OData
