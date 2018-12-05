#include "SearchQuery.h"

#include "ProductAttribute.h"
#include <gtest/gtest.h>

namespace OData {
namespace Test {

TEST(SearchQueryTest, KeywordTest) {
  {
    SearchQuery simple_query(Attribute::PLATFORM, "y");
    ASSERT_EQ("platformname:y", simple_query.getQuery());
  }
  {
    SearchQuery simple_query(Attribute::CREATION_DATE, "y");
    ASSERT_EQ("beginposition:y", simple_query.getQuery());
  }
  {
    SearchQuery simple_query(Attribute::END_POSITION, "y");
    ASSERT_EQ("endposition:y", simple_query.getQuery());
  }
  {
    SearchQuery simple_query(Attribute::INGESTION_DATE, "y");
    ASSERT_EQ("ingestiondate:y", simple_query.getQuery());
  }
  {
    SearchQuery simple_query(Attribute::COLLECTION, "y");
    ASSERT_EQ("collection:y", simple_query.getQuery());
  }
  {
    SearchQuery simple_query(Attribute::FILENAME, "y");
    ASSERT_EQ("filename:y", simple_query.getQuery());
  }
  {
    SearchQuery simple_query(Attribute::FOOTPRINT, "y");
    ASSERT_EQ("footprint:y", simple_query.getQuery());
  }
  {
    SearchQuery simple_query(Attribute::ORBIT_NUMBER, "y");
    ASSERT_EQ("orbitnumber:y", simple_query.getQuery());
  }
  {
    SearchQuery simple_query(Attribute::LAST_ORBIT_NUMBER, "y");
    ASSERT_EQ("lastorbitnumber:y", simple_query.getQuery());
  }
  {
    SearchQuery simple_query(Attribute::RELATIVE_ORBIT_NUMBER, "y");
    ASSERT_EQ("relativeorbitnumber:y", simple_query.getQuery());
  }
  {
    SearchQuery simple_query(Attribute::LAST_RELATIVE_ORBIT_NUMBER, "y");
    ASSERT_EQ("lastrelativeorbitnumber:y", simple_query.getQuery());
  }
  {
    SearchQuery simple_query(Attribute::ORBIT_DIRECTION, "y");
    ASSERT_EQ("orbitdirection:y", simple_query.getQuery());
  }
  {
    SearchQuery simple_query(Attribute::POLARISATION_MODE, "y");
    ASSERT_EQ("polarisationmode:y", simple_query.getQuery());
  }
  {
    SearchQuery simple_query(Attribute::PRODUCT_TYPE, "y");
    ASSERT_EQ("producttype:y", simple_query.getQuery());
  }
  {
    SearchQuery simple_query(Attribute::SENSOR_OPER_MODE, "y");
    ASSERT_EQ("sensoroperationalmode:y", simple_query.getQuery());
  }
  {
    SearchQuery simple_query(Attribute::SWATH_ID, "y");
    ASSERT_EQ("swathidentifier:y", simple_query.getQuery());
  }
  {
    SearchQuery simple_query(Attribute::CLOUD_COVERAGE, "y");
    ASSERT_EQ("cloudcoverpercentage:y", simple_query.getQuery());
  }
}

TEST(SearchQueryTest, CreateQueryTest) {
  SearchQuery empty;
  ASSERT_EQ("", empty.getQuery());

  SearchQuery simple_query(Attribute::PLATFORM, "y");
  ASSERT_EQ("platformname:y", simple_query.getQuery());

  SearchQuery simple_int_query(Attribute::PLATFORM, 6);
  ASSERT_EQ("platformname:6", simple_int_query.getQuery());

  SearchQuery and_query(
      {Attribute::PLATFORM, "y"},
      {Attribute::FILENAME, "b"},
      SearchQuery::Operator::AND);
  ASSERT_EQ("(platformname:y%20AND%20filename:b)", and_query.getQuery());

  SearchQuery or_query(
      {Attribute::PLATFORM, "y"},
      {Attribute::FILENAME, "b"},
      SearchQuery::Operator::OR);
  ASSERT_EQ("(platformname:y%20OR%20filename:b)", or_query.getQuery());

  SearchQuery not_query(
      {Attribute::PLATFORM, "y"},
      {Attribute::FILENAME, 7},
      SearchQuery::Operator::NOT);
  ASSERT_EQ("(platformname:y%20NOT%20filename:7)", not_query.getQuery());
}

} // namespace Test
} // namespace OData
