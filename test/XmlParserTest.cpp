#include <gtest/gtest.h>
#include <fstream>
#include <stdexcept>
#include <sstream>
#include "Entry.h"
#include "XmlParser.h"

namespace {
std::string readTestInstance(
    const std::string& filename) {
    std::fstream file("examples/" + filename, std::fstream::in);
    if(file.is_open()) {
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
TEST(XmlParserTest, TestListParser) {
    XmlParser parser;
    const auto entries = parser.parseList(readTestInstance("list.xml"));
    ASSERT_EQ(5, entries.size());
    ASSERT_THROW(parser.parseList(readTestInstance("attribute-filename.xml")), std::invalid_argument);
}

TEST(XmlParserTest, TestFilenameParser) {
    XmlParser parser;
    ASSERT_EQ("S2A_MSIL2A_20170620T100031_N0205_R122_T33UWR_20170620T100453.SAFE",
            parser.parseFilename(readTestInstance("attribute-filename.xml")));
    ASSERT_EQ("S3A_SL_1_RBT____20170326T102022_20170326T102322_20170326T122723_0179_016_008_2159_SVL_O_NR_002.SEN3",
            parser.parseFilename(readTestInstance("attributes.xml")));
    ASSERT_THROW(parser.parseFilename(readTestInstance("list.xml")), std::invalid_argument);
}

TEST(XmlParserTest, TestManifestParser) {
    XmlParser parser;
    const auto entries = parser.parseManifest(readTestInstance("manifest.xml"));
    ASSERT_EQ(88, entries.size());
}

} /* namespace Test */
} /* namespace OData */
