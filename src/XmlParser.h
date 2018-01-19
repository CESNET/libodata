#ifndef SRC_XMLPARSER_H_
#define SRC_XMLPARSER_H_

#include <string>
#include <vector>

namespace OData {

class Entry;

class XmlParser {
public:
    std::vector<Entry> parseList(
        const std::string& xml) const;
};

} /* namespace OData */

#endif /* SRC_XMLPARSER_H_ */
