#ifndef SRC_ARCHIVE_H_
#define SRC_ARCHIVE_H_

#include "Directory.h"
#include "FileSystemNode.h"
#include "ProductFile.h"
#include "ProductPath.h"
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <memory>

namespace OData {

class Archive : public FileSystemNode, public ProductFile {
public:
  Archive() = default;
  Archive(
      std::shared_ptr<Directory> content,
      ProductPath path,
      std::size_t size) noexcept;
  virtual ~Archive() = default;

  void toString(std::ostream& ostr, unsigned indent_level = 0) const
      noexcept override;
  bool compare(const FileSystemNode& node) const noexcept override;
  std::string getName() const noexcept override;
  std::shared_ptr<FileSystemNode> getFile(
      boost::filesystem::path::const_iterator begin,
      boost::filesystem::path::const_iterator end) const noexcept override;
  std::vector<std::string> readDir() const noexcept override;
  bool isDirectory() const noexcept override;
  std::size_t getSize() const noexcept override;
  ProductPath getProductPath() const noexcept override;

private:
  friend class boost::serialization::access;
  template <typename Archive> void serialize(Archive& ar, const unsigned int) {
    ar& boost::serialization::base_object<FileSystemNode>(*this);
    ar& content;
    ar& path;
    ar& size;
  }

  std::shared_ptr<Directory> content;
  ProductPath path;
  std::size_t size;
};

} /* namespace OData */

BOOST_CLASS_EXPORT_KEY(OData::Archive)

#endif /* SRC_ARCHIVE_H_ */
