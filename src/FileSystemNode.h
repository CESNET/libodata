#ifndef SRC_FILESYSTEMNODE_H_
#define SRC_FILESYSTEMNODE_H_

#include <iosfwd>
#include <memory>
#include <vector>

namespace OData {

class Product;

class FileSystemNode {
public:
  FileSystemNode() = default;
  virtual ~FileSystemNode() = default;
  FileSystemNode(const FileSystemNode&) = delete;
  FileSystemNode& operator=(const FileSystemNode&) = delete;
  bool operator==(const FileSystemNode& node) const;
  bool operator!=(const FileSystemNode& node) const;

  virtual void toString(std::ostream& ostr, unsigned indent_level = 0) const
      noexcept = 0;
  virtual bool compare(const FileSystemNode& node) const noexcept = 0;
  virtual std::string getName() const noexcept = 0;

  static std::ostream& indent(std::ostream& ostr, unsigned level);
  static std::unique_ptr<FileSystemNode> create(
      std::vector<std::unique_ptr<Product>> products) noexcept;
};

} /* namespace OData */

#endif /* SRC_FILESYSTEMNODE_H_ */
