#ifndef SRC_REMOTEFILE_H_
#define SRC_REMOTEFILE_H_

#include "FileSystemNode.h"
#include "ProductFile.h"
#include "ProductPath.h"
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#include <string>

namespace OData {

class RemoteFile : public FileSystemNode, public ProductFile {
public:
  RemoteFile() = default;
  RemoteFile(std::string name, ProductPath path, std::size_t size) noexcept;
  virtual ~RemoteFile() = default;
  RemoteFile(const RemoteFile&) = delete;
  RemoteFile& operator=(const RemoteFile&) = delete;

  void toString(std::ostream& ostr, unsigned indent_level = 0) const override;
  bool compare(const FileSystemNode& node) const noexcept override;
  std::string getName() const noexcept;
  std::shared_ptr<FileSystemNode> getFile(
      boost::filesystem::path::const_iterator begin,
      boost::filesystem::path::const_iterator end) const noexcept override;
  std::vector<std::string> readDir() const noexcept override;
  bool isDirectory() const noexcept override;
  std::size_t getSize() const noexcept override;
  ProductPath getProductPath() const noexcept override;
  std::shared_ptr<FileSystemNode> getChild(const std::string& name) const
      noexcept override;
  void removeChild(const std::string& child_name) noexcept override;
  void addChild(std::shared_ptr<FileSystemNode> child) noexcept override;

private:
  friend class boost::serialization::access;
  template <typename Archive> void serialize(Archive& ar, const unsigned int) {
    ar& boost::serialization::base_object<FileSystemNode>(*this);
    ar& name;
    ar& path;
    ar& size;
  }

  std::string name;
  ProductPath path;
  std::size_t size;
};

} /* namespace OData */

BOOST_CLASS_EXPORT_KEY(OData::RemoteFile)

#endif /* SRC_REMOTEFILE_H_ */
