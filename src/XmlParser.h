#ifndef SRC_XMLPARSER_H_
#define SRC_XMLPARSER_H_

#include <boost/filesystem/path.hpp>
#include <memory>
#include <vector>

namespace OData {

class Product;

/**
 * Parser for xml responses sent by data hub server
 */
class XmlParser {
public:
  XmlParser() = default;
  ~XmlParser() = default;
  XmlParser(const XmlParser&) = delete;
  XmlParser& operator=(const XmlParser&) = delete;

  /**
   * Parse product list query
   * @return products
   */
  std::vector<std::shared_ptr<Product>> parseList(
      const std::vector<char>& xml) const;

  /**
   * Parse product manifest file
   * @return product files and their sizes
   */
  std::vector<std::pair<boost::filesystem::path, std::size_t>> parseManifest(
      const std::vector<char>& manifest) const;

  /**
   * Parse deleted products query
   * @return deleted products ids
   */
  std::vector<std::string> parseDeletedList(const std::vector<char>& xml) const;
};

} /* namespace OData */

#endif /* SRC_XMLPARSER_H_ */
