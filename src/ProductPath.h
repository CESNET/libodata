#ifndef SRC_PRODUCTPATH_H_
#define SRC_PRODUCTPATH_H_

#include <boost/filesystem/path.hpp>
#include <string>

namespace boost {
namespace filesystem {
// TODO workaround, put somewhere else
template <class Archive>
void serialize(Archive& ar, boost::filesystem::path& path, const unsigned int) {
  std::string raw = path.string();
  ar& raw;
  path = boost::filesystem::path(raw);
}
} // namespace filesystem

namespace serialization {
class access;
} // namespace serialization
} // namespace boost

namespace OData {

class ProductPath {
public:
  ProductPath() = default;
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
  friend class boost::serialization::access;
  template <typename Archive> void serialize(Archive& ar, const unsigned int) {
    ar& uuid;
    ar& filename;
    ar& path;
  }

  std::string uuid;
  std::string filename;
  boost::filesystem::path path;
};

} /* namespace OData */

#endif /* SRC_PRODUCTPATH_H_ */
