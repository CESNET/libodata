#ifndef SRC_PATHBUILDER_H_
#define SRC_PATHBUILDER_H_

#include <boost/filesystem/path.hpp>
#include <memory>
#include <string>

namespace OData {

class Product;

/**
 * Product filesystem path builder based on configuration template.
 * String ${keyword} are replaced by property "keyword" value or by string
 * "unknown" if there is no such property.
 */
class PathBuilder {
public:
  PathBuilder();
  PathBuilder(const std::string& path_template);
  PathBuilder(PathBuilder&&);
  ~PathBuilder();
  PathBuilder& operator=(PathBuilder&&);

  /**
   * Create product path
   * @return product parent directory
   */
  boost::filesystem::path createPath(const Product& product) const noexcept;

private:
  struct Impl;
  std::unique_ptr<Impl> pimpl;
};

} /* namespace OData */

#endif /* SRC_PATHBUILDER_H_ */
