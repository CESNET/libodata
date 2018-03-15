#ifndef SRC_PRODUCTPATH_H_
#define SRC_PRODUCTPATH_H_

#include <boost/filesystem/path.hpp>
#include <string>

namespace OData {

class ProductPath {
public:
  ProductPath(std::string uuid) noexcept;
  ProductPath(std::string uuid, std::string filename) noexcept;
  ProductPath(
      std::string uuid,
      std::string filename,
      boost::filesystem::path path) noexcept;
  ProductPath(const ProductPath& path, std::string appended_path) noexcept;
  ~ProductPath() = default;
  ProductPath(const ProductPath&) = default;
  ProductPath(ProductPath&&) = default;
  ProductPath& operator=(const ProductPath&) = default;
  ProductPath& operator=(ProductPath&&) = default;

  void append(std::string path) noexcept;
  std::string getPath() const noexcept;
  bool operator==(const ProductPath& other) const noexcept;

private:
  std::string uuid;
  std::string filename;
  boost::filesystem::path path;
};

} /* namespace OData */

#endif /* SRC_PRODUCTPATH_H_ */
