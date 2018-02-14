#ifndef SRC_XMLPARSER_H_
#define SRC_XMLPARSER_H_

#include <memory>
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
  std::vector<std::unique_ptr<Product>> parseList(
      const std::vector<char>& xml) const;
  std::vector<std::string> parseManifest(
      const std::vector<char>& manifest) const;
};

} /* namespace OData */

#endif /* SRC_XMLPARSER_H_ */
