#include "SearchQueryBuilder.h"
#include <gtest/gtest.h>

namespace OData {
namespace Test {

TEST(SearchQueryBuilderTest, SearchQueryTest) {
  SearchQuery empty;
  ASSERT_EQ("", empty.getQuery());

  SearchQuery simple_query("x", "y");
  ASSERT_EQ("x:y", simple_query.getQuery());

  SearchQuery simple_int_query("x", 6);
  ASSERT_EQ("x:6", simple_int_query.getQuery());

  SearchQuery and_query({"x", "y"}, {"a", "b"}, SearchQuery::Operator::AND);
  ASSERT_EQ("(x:y%20AND%20a:b)", and_query.getQuery());

  SearchQuery or_query({"x", "y"}, {"a", "b"}, SearchQuery::Operator::OR);
  ASSERT_EQ("(x:y%20OR%20a:b)", or_query.getQuery());

  SearchQuery not_query({"x", "y"}, {"a", 7}, SearchQuery::Operator::NOT);
  ASSERT_EQ("(x:y%20NOT%20a:7)", not_query.getQuery());
}

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
  simple.setQuery({"x", "y"});
  ASSERT_EQ(
      "search?start=0&rows=10&orderby=ingestiondate%20desc&q=x:y",
      simple.build());

  SearchQueryBuilder complex;
  complex.setQuery({{"x", "y"}, {"a", 42}, SearchQuery::Operator::AND});
  complex.setOrder("y", false);
  complex.setRows(100);
  complex.setStart(100);
  ASSERT_EQ(
      "search?start=100&rows=100&orderby=y%20desc&q=(x:y%20AND%20a:42)",
      complex.build());
}

} // namespace Test
} // namespace OData
