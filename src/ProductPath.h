#ifndef SRC_PRODUCTPATH_H_
#define SRC_PRODUCTPATH_H_

#include "SerializationHelpers.h"
#include <boost/filesystem/path.hpp>
#include <string>

namespace boost {
namespace serialization {
class access;
} // namespace serialization
} // namespace boost

namespace OData {

/**
 * Path to product file used by OData protocol.
 */
class ProductPath {
public:
  ProductPath() = default;

  /**
   * Constructor
   * @param uuid product identifier
   */
  ProductPath(std::string uuid) noexcept;

  /**
   * Constructor
   * @param uuid product identifier
   * @param filename product filename
   */
  ProductPath(std::string uuid, std::string filename) noexcept;

  /**
   * Constructor
   * @param uuid product identifier
   * @param filename product filename
   * @param file_path path to file stored in product archive
   */
  ProductPath(
      std::string uuid,
      std::string filename,
      boost::filesystem::path file_path) noexcept;
  ProductPath(const ProductPath& path, std::string appended_path) noexcept;
  ~ProductPath() = default;
  ProductPath(const ProductPath&) = default;
  ProductPath(ProductPath&&) = default;
  ProductPath& operator=(const ProductPath&) = default;
  ProductPath& operator=(ProductPath&&) = default;

  /**
   * Append path node to file_path
   */
  void append(std::string path) noexcept;

  /**
   * @return path in format used by OData protocol:
   * Products('uuid')/Node('filename')/Node('path)/Node('to')/Node('file')/$value
   */
  std::string getPath() const noexcept;
  bool operator==(const ProductPath& other) const noexcept;
  bool operator<(const ProductPath& other) const noexcept;

private:
  friend class boost::serialization::access;
  template <typename Archive> void serialize(Archive& ar, const unsigned int) {
    ar& uuid;
    ar& filename;
    ar& file_path;
  }

  std::string uuid;
  std::string filename;
  boost::filesystem::path file_path;
};

} /* namespace OData */

#endif /* SRC_PRODUCTPATH_H_ */
