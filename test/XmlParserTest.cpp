#include <gtest/gtest.h>
#include <fstream>
#include <stdexcept>
#include <sstream>
#include "Product.h"
#include "XmlParser.h"

namespace {
std::string readTestInstance(
    const std::string& filename) {
    std::fstream file("examples/" + filename, std::fstream::in);
    if (file.is_open()) {
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    } else {
        throw std::invalid_argument("Cannot open file" + filename);
    }
}
}

namespace OData {
namespace Test {

TEST(XmlParserTest, TestManifestParser) {
    XmlParser parser;
    const auto entries = parser.parseManifest(readTestInstance("manifest.xml"));
    ASSERT_EQ(88, entries.size());
}

TEST(XmlParserTest, TestListResponseParser) {
    XmlParser parser;
    const auto entries = parser.parseList(readTestInstance("search.xml"));
    ASSERT_EQ(5, entries.size());
    ASSERT_EQ(Product("63a6c50d-1bba-45eb-a7db-85ecd334e30b",
            "S1B_IW_SLC__1SDV_20180121T165029_20180121T165056_009272_010A05_11E9",
            "2018-01-22T23:06:09.235Z",
            "S1B_IW_SLC__1SDV_20180121T165029_20180121T165056_009272_010A05_11E9.SAFE"),
            entries[0]);
    ASSERT_EQ(Product("725adbf7-dd68-49c2-b466-061fa5b07861",
                "S1B_IW_SLC__1SDV_20180121T165053_20180121T165120_009272_010A05_A393",
                "2018-01-22T23:05:33.262Z",
                "S1B_IW_SLC__1SDV_20180121T165053_20180121T165120_009272_010A05_A393.SAFE"),
                entries[3]);
}

} /* namespace Test */
} /* namespace OData */
