#ifndef SRC_XMLPARSER_H_
#define SRC_XMLPARSER_H_

#include <boost/filesystem/path.hpp>
#include <memory>
#include <vector>

namespace OData {

class Product;

class XmlParser {
public:
  XmlParser() = default;
  ~XmlParser() = default;
  XmlParser(const XmlParser&) = delete;
  XmlParser& operator=(const XmlParser&) = delete;
  std::vector<std::shared_ptr<Product>> parseList(
      const std::vector<char>& xml) const;
  std::vector<std::pair<boost::filesystem::path, std::size_t>> parseManifest(
      const std::vector<char>& manifest) const;
  std::vector<std::string> parseDeletedList(const std::vector<char>& xml) const;
};

} /* namespace OData */

#endif /* SRC_XMLPARSER_H_ */
