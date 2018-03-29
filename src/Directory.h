#ifndef SRC_DIRECTORY_H_
#define SRC_DIRECTORY_H_

#include "FileSystemNode.h"
#include <boost/filesystem/path.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <iosfwd>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace OData {

class Product;
class ProductPath;

class Directory : public FileSystemNode {
public:
  using Content = std::map<std::string, std::shared_ptr<FileSystemNode>>;

  Directory() = default;
  Directory(std::string name, Content content = {}) noexcept;
  virtual ~Directory() = default;
  Directory(const Directory&) = delete;
  Directory& operator=(const Directory&) = delete;
  void toString(std::ostream& ostr, unsigned indent_level = 0) const
      noexcept override;
  bool compare(const FileSystemNode& node) const noexcept override;
  std::string getName() const noexcept override;
  const FileSystemNode* getFile(
      boost::filesystem::path::const_iterator begin,
      boost::filesystem::path::const_iterator end) const noexcept override;
  std::vector<std::string> readDir() const noexcept override;
  bool isDirectory() const noexcept override;

  void addChild(std::shared_ptr<FileSystemNode> child) noexcept;
  FileSystemNode* getChild(const std::string& name) noexcept;

  static std::unique_ptr<Directory> createRemoteStructure(
      const ProductPath& product_path,
      std::string name,
      const std::vector<boost::filesystem::path>& files) noexcept;

  void appendProducts(std::vector<std::shared_ptr<Product>> products);

  static std::unique_ptr<Directory> createFilesystem(
      std::vector<std::shared_ptr<Product>> products) noexcept;

private:
  friend class boost::serialization::access;
  template <typename Archive> void serialize(Archive& ar, const unsigned int) {
    ar& boost::serialization::base_object<FileSystemNode>(*this);
    ar& name;
    ar& content;
  }

  std::string name;
  Content content;
  mutable boost::shared_mutex content_mutex;
};

std::ostream& operator<<(
    std::ostream& ostr, const Directory& directory) noexcept;

} /* namespace OData */

BOOST_CLASS_EXPORT_KEY(OData::Directory)

#endif /* SRC_DIRECTORY_H_ */
