#include "XmlParser.h"

#include "Product.h"
#include "Utils.h"
#include <gtest/gtest.h>

namespace OData {
namespace Test {

TEST(XmlParserTest, TestManifestParser) {
  XmlParser parser;
  {
    const auto entries = parser.parseManifest(readTestInstance("manifest.xml"));
    ASSERT_EQ(88, entries.size());
  }
  {
    const auto entries =
        parser.parseManifest(readTestInstance("xfdumanifest.xml"));
    ASSERT_EQ(110, entries.size());
  }
}

TEST(XmlParserTest, TestListResponseParser) {
  XmlParser parser;
  const auto entries = parser.parseList(readTestInstance("search.xml"));
  ASSERT_EQ(5, entries.size());

  // entry[0]
  ASSERT_EQ("63a6c50d-1bba-45eb-a7db-85ecd334e30b", entries[0]->getId());
  ASSERT_EQ(
      "S1B_IW_SLC__1SDV_20180121T165029_20180121T165056_009272_010A05_11E9",
      entries[0]->getName());
  ASSERT_EQ("2018-01-21", entries[0]->getDate());
  ASSERT_EQ(
      "S1B_IW_SLC__1SDV_20180121T165029_20180121T165056_009272_010A05_11E9."
      "SAFE",
      entries[0]->getFilename());
  ASSERT_EQ(7700000000UL, entries[0]->getSize());

  // entry[3]
  ASSERT_EQ("725adbf7-dd68-49c2-b466-061fa5b07861", entries[3]->getId());
  ASSERT_EQ(
      "S1B_IW_SLC__1SDV_20180121T165053_20180121T165120_009272_010A05_A393",
      entries[3]->getName());
  ASSERT_EQ("2018-01-21", entries[3]->getDate());
  ASSERT_EQ(
      "S1B_IW_SLC__1SDV_20180121T165053_20180121T165120_009272_010A05_A393."
      "SAFE",
      entries[3]->getFilename());
  ASSERT_EQ(7700000UL, entries[3]->getSize());
}

TEST(XmlParserTest, TestDeleteQueryParser) {
  XmlParser parser;
  const auto entries =
      parser.parseDeletedList(readTestInstance("deletedproducts.xml"));
  ASSERT_EQ(50, entries.size());
  ASSERT_EQ("2a7e89c0-67c9-482b-9d59-ecdec4f43ea7", entries[0]);
}

} /* namespace Test */
} /* namespace OData */
