#ifndef SRC_PRODUCTPATH_H_
#define SRC_PRODUCTPATH_H_

#include <string>
#include <vector>

namespace OData {

class ProductPath {
public:
  ProductPath(std::string uuid, std::string filename) noexcept;
  ProductPath(
      std::string uuid,
      std::string filename,
      std::initializer_list<std::string> path) noexcept;
  ~ProductPath() = default;
  ProductPath(const ProductPath&) = default;
  ProductPath(ProductPath&&) = default;
  ProductPath& operator=(const ProductPath&) = default;
  ProductPath& operator=(ProductPath&&) = default;

  void appendPath(std::initializer_list<std::string> path) noexcept;
  std::string getPath() const noexcept;

private:
  std::string uuid;
  std::string filename;
  std::vector<std::string> path;
};

} /* namespace OData */

#endif /* SRC_PRODUCTPATH_H_ */
