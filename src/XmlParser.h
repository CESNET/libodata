#ifndef SRC_XMLPARSER_H_
#define SRC_XMLPARSER_H_

#include <string>
#include <vector>

namespace OData {

class Product;

class XmlParser {
public:
  XmlParser() = default;
  ~XmlParser() = default;
  XmlParser(const XmlParser&) = delete;
  XmlParser& operator=(const XmlParser&) = delete;
  std::vector<Product> parseList(const std::string& xml) const;
  std::vector<std::string> parseManifest(const std::string& manifest) const;
};

} /* namespace OData */

#endif /* SRC_XMLPARSER_H_ */
