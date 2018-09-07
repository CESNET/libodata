#ifndef SRC_PRODUCT_H_
#define SRC_PRODUCT_H_

#include "Directory.h"
#include "FileSystemNode.h"
#include "ProductPath.h"
#include <atomic>
#include <boost/optional/optional.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <cstdlib>
#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

namespace OData {

class File;
class RemoteFile;

/**
 * Product stored in Copernicus Open Access Hub
 */
class Product : public FileSystemNode {
public:
  Product() = default;
  Product(std::map<std::string, std::string> attributes) noexcept;
  Product(const Product&) = delete;
  virtual ~Product() = default;
  Product& operator=(const Product&) = delete;

  void setArchiveStructure(
      std::shared_ptr<Directory> directory,
      std::shared_ptr<File> manifest) noexcept;
  ProductPath getProductPath() const noexcept;
  ProductPath getArchivePath() const noexcept;

  void toString(std::ostream& ostr, unsigned indent_level = 0) const override;
  bool compare(const FileSystemNode& node) const noexcept override;
  std::string getName() const noexcept override;
  std::shared_ptr<FileSystemNode> getFile(
      boost::filesystem::path::const_iterator begin,
      boost::filesystem::path::const_iterator end) const noexcept override;
  std::vector<std::string> readDir() const noexcept override;
  bool isDirectory() const noexcept override;
  std::size_t getSize() const noexcept override;
  std::shared_ptr<FileSystemNode> getChild(
      const std::string& name) const noexcept override;
  void removeChild(const std::string& child_name) noexcept override;
  void addChild(std::shared_ptr<FileSystemNode> child) noexcept override;
  std::string getPlatform() const noexcept;
  std::string getManifestFilename() const noexcept;
  std::string getFilename() const noexcept;
  std::string getDate() const noexcept;
  std::string getId() const noexcept;
  boost::optional<std::string> getAttribute(const std::string& name) const
      noexcept;

private:
  bool isArchiveSet() const;
  std::string getRequiredAttribute(const std::string& name) const noexcept;
  boost::optional<std::string> getDateAttribute(unsigned attribute) const
      noexcept;

  friend class boost::serialization::access;
  template <typename Archive> void serialize(Archive& ar, const unsigned int) {
    ar& boost::serialization::base_object<FileSystemNode>(*this);
    std::uint32_t version = 1;
    ar& version;
    if (version == 1) {
      ar& attributes;
      ar& directory;
      ar& manifest;
      ar& archive;
    }
  }
  std::map<std::string, std::string> attributes;
  std::shared_ptr<FileSystemNode> directory;
  std::shared_ptr<File> manifest;
  std::shared_ptr<RemoteFile> archive;
};

std::ostream& operator<<(std::ostream& ostr, const Product& product) noexcept;

} /* namespace OData */

BOOST_CLASS_EXPORT_KEY(OData::Product)

#endif /* SRC_PRODUCT_H_ */
