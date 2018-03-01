#ifndef SRC_DIRECTORY_H_
#define SRC_DIRECTORY_H_

#include "FileSystemNode.h"
#include <iosfwd>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace OData {

class Product;

class Directory : public FileSystemNode {
public:
  using Content = std::map<std::string, std::unique_ptr<FileSystemNode>>;

  Directory(std::string name, Content content = {}) noexcept;
  virtual ~Directory() = default;
  Directory(const Directory&) = delete;
  Directory& operator=(const Directory&) = delete;
  void toString(std::ostream& ostr, unsigned indent_level = 0) const
      noexcept override;
  bool compare(const FileSystemNode& node) const noexcept override;
  std::string getName() const noexcept override;
  FileSystemNode* getFile(std::list<std::string> path) const noexcept override;
  std::vector<std::string> readDir() const noexcept override;

  void addChild(std::unique_ptr<FileSystemNode> child) noexcept;
  void addFile(std::string file) noexcept;
  FileSystemNode* getChild(const std::string& name) noexcept;

  static std::unique_ptr<Directory> create(
      std::string name, const std::vector<std::string>& files) noexcept;

  void appendProducts(std::vector<std::unique_ptr<Product>> products);

  static std::unique_ptr<Directory> createFilesystem(
      std::vector<std::unique_ptr<Product>> products) noexcept;

private:
  std::string name;
  Content content;
};

std::ostream& operator<<(
    std::ostream& ostr, const Directory& directory) noexcept;

} /* namespace OData */

#endif /* SRC_DIRECTORY_H_ */
