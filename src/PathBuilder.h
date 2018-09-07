#ifndef SRC_PATHBUILDER_H_
#define SRC_PATHBUILDER_H_

#include <boost/filesystem/path.hpp>
#include <memory>
#include <string>

namespace OData {

class Product;

class PathBuilder {
public:
  PathBuilder();
  PathBuilder(const std::string& path_template);
  PathBuilder(PathBuilder&&);
  ~PathBuilder();
  PathBuilder& operator=(PathBuilder&&);

  boost::filesystem::path createPath(const Product& product) const noexcept;

private:
  struct Impl;
  std::unique_ptr<Impl> pimpl;
};

} /* namespace OData */

#endif /* SRC_PATHBUILDER_H_ */
