#include "SearchQuery.h"
#include <gtest/gtest.h>

namespace OData {
namespace Test {

TEST(SearchQueryTest, KeywordTest) {
  {
    SearchQuery simple_query(SearchQuery::Keyword::PLATFORM, "y");
    ASSERT_EQ("platformname:y", simple_query.getQuery());
  }
  {
    SearchQuery simple_query(SearchQuery::Keyword::BEGIN_POSITION, "y");
    ASSERT_EQ("beginposition:y", simple_query.getQuery());
  }
  {
    SearchQuery simple_query(SearchQuery::Keyword::END_POSITION, "y");
    ASSERT_EQ("endposition:y", simple_query.getQuery());
  }
  {
    SearchQuery simple_query(SearchQuery::Keyword::INGESTION_DATE, "y");
    ASSERT_EQ("ingestiondate:y", simple_query.getQuery());
  }
  {
    SearchQuery simple_query(SearchQuery::Keyword::COLLECTION, "y");
    ASSERT_EQ("collection:y", simple_query.getQuery());
  }
  {
    SearchQuery simple_query(SearchQuery::Keyword::FILENAME, "y");
    ASSERT_EQ("filename:y", simple_query.getQuery());
  }
  {
    SearchQuery simple_query(SearchQuery::Keyword::FOOTPRINT, "y");
    ASSERT_EQ("footprint:y", simple_query.getQuery());
  }
  {
    SearchQuery simple_query(SearchQuery::Keyword::ORBIT_NUMBER, "y");
    ASSERT_EQ("orbitnumber:y", simple_query.getQuery());
  }
  {
    SearchQuery simple_query(SearchQuery::Keyword::LAST_ORBIT_NUMBER, "y");
    ASSERT_EQ("lastorbitnumber:y", simple_query.getQuery());
  }
  {
    SearchQuery simple_query(SearchQuery::Keyword::RELATIVE_ORBIT_NUMBER, "y");
    ASSERT_EQ("relativeorbitnumber:y", simple_query.getQuery());
  }
  {
    SearchQuery simple_query(
        SearchQuery::Keyword::LAST_RELATIVE_ORBIT_NUMBER, "y");
    ASSERT_EQ("lastrelativeorbitnumber:y", simple_query.getQuery());
  }
  {
    SearchQuery simple_query(SearchQuery::Keyword::ORBIT_DIRECTION, "y");
    ASSERT_EQ("orbitdirection:y", simple_query.getQuery());
  }
  {
    SearchQuery simple_query(SearchQuery::Keyword::POLARISATION_MODE, "y");
    ASSERT_EQ("polarisationmode:y", simple_query.getQuery());
  }
  {
    SearchQuery simple_query(SearchQuery::Keyword::PRODUCT_TYPE, "y");
    ASSERT_EQ("producttype:y", simple_query.getQuery());
  }
  {
    SearchQuery simple_query(SearchQuery::Keyword::SENSOR_OPER_MODE, "y");
    ASSERT_EQ("sensoroperationalmode:y", simple_query.getQuery());
  }
  {
    SearchQuery simple_query(SearchQuery::Keyword::SWATH_ID, "y");
    ASSERT_EQ("swathidentifier:y", simple_query.getQuery());
  }
  {
    SearchQuery simple_query(SearchQuery::Keyword::CLOUD_COVERAGE, "y");
    ASSERT_EQ("cloudcoverpercentage:y", simple_query.getQuery());
  }
}

TEST(SearchQueryTest, CreateQueryTest) {
  SearchQuery empty;
  ASSERT_EQ("", empty.getQuery());

  SearchQuery simple_query(SearchQuery::Keyword::PLATFORM, "y");
  ASSERT_EQ("platformname:y", simple_query.getQuery());

  SearchQuery simple_int_query(SearchQuery::Keyword::PLATFORM, 6);
  ASSERT_EQ("platformname:6", simple_int_query.getQuery());

  SearchQuery and_query(
      {SearchQuery::Keyword::PLATFORM, "y"},
      {SearchQuery::Keyword::FILENAME, "b"},
      SearchQuery::Operator::AND);
  ASSERT_EQ("(platformname:y%20AND%20filename:b)", and_query.getQuery());

  SearchQuery or_query(
      {SearchQuery::Keyword::PLATFORM, "y"},
      {SearchQuery::Keyword::FILENAME, "b"},
      SearchQuery::Operator::OR);
  ASSERT_EQ("(platformname:y%20OR%20filename:b)", or_query.getQuery());

  SearchQuery not_query(
      {SearchQuery::Keyword::PLATFORM, "y"},
      {SearchQuery::Keyword::FILENAME, 7},
      SearchQuery::Operator::NOT);
  ASSERT_EQ("(platformname:y%20NOT%20filename:7)", not_query.getQuery());
}

} // namespace Test
} // namespace OData
