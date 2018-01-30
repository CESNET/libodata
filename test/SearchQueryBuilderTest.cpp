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
  ASSERT_EQ("(x:y AND a:b)", and_query.getQuery());

  SearchQuery or_query({"x", "y"}, {"a", "b"}, SearchQuery::Operator::OR);
  ASSERT_EQ("(x:y OR a:b)", or_query.getQuery());

  SearchQuery not_query({"x", "y"}, {"a", 7}, SearchQuery::Operator::NOT);
  ASSERT_EQ("(x:y NOT a:7)", not_query.getQuery());
}

TEST(SearchQueryBuilderTest, SearchQueryBuilderTest) {
  SearchQueryBuilder empty;
  ASSERT_EQ("search?q=", empty.build());

  SearchQueryBuilder simple;
  simple.addQuery({"x", "y"});
  ASSERT_EQ("search?q=x%3Ay", simple.build());

  SearchQueryBuilder complex;
  complex.addQuery({{"x", "y"}, {"a", 42}, SearchQuery::Operator::AND});
  ASSERT_EQ("search?q=%28x%3Ay%20AND%20a%3A42%29", complex.build());

  SearchQueryBuilder multiple_queries;
  multiple_queries.addQuery(
      {{"x", "y"}, {"a", "b"}, SearchQuery::Operator::AND});
  multiple_queries.addQuery({"z", "w"});
  ASSERT_EQ(
      "search?q=%28x%3Ay%20AND%20a%3Ab%29&z%3Aw", multiple_queries.build());
}

} // namespace Test
} // namespace OData
