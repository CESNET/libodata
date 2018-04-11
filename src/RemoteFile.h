#ifndef SRC_REMOTEFILE_H_
#define SRC_REMOTEFILE_H_

#include "FileSystemNode.h"
#include "ProductPath.h"
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#include <string>

namespace OData {

class RemoteFile : public FileSystemNode {
public:
  RemoteFile() = default;
  RemoteFile(std::string name, ProductPath path) noexcept;
  virtual ~RemoteFile() = default;
  RemoteFile(const RemoteFile&) = delete;
  RemoteFile& operator=(const RemoteFile&) = delete;

  void toString(std::ostream& ostr, unsigned indent_level = 0) const
      noexcept override;
  bool compare(const FileSystemNode& node) const noexcept override;
  std::string getName() const noexcept;
  std::shared_ptr<FileSystemNode> getFile(
      boost::filesystem::path::const_iterator begin,
      boost::filesystem::path::const_iterator end) const noexcept override;
  std::vector<std::string> readDir() const noexcept override;
  bool isDirectory() const noexcept override;

  ProductPath getProductPath() const noexcept;

private:
  friend class boost::serialization::access;
  template <typename Archive> void serialize(Archive& ar, const unsigned int) {
    ar& boost::serialization::base_object<FileSystemNode>(*this);
    ar& name;
    ar& path;
  }

  std::string name;
  ProductPath path;
};

} /* namespace OData */

BOOST_CLASS_EXPORT_KEY(OData::RemoteFile)

#endif /* SRC_REMOTEFILE_H_ */
