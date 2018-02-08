#ifndef SRC_DIRECTORY_H_
#define SRC_DIRECTORY_H_

#include "FileSystemNode.h"
#include <iosfwd>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace OData {

class Directory : public FileSystemNode {
public:
  using Subdirectories = std::map<std::string, std::unique_ptr<FileSystemNode>>;

  explicit Directory(std::string name);
  Directory(
      std::string name,
      std::vector<std::string> files,
      Subdirectories sub_directories = {}) noexcept;
  virtual ~Directory() = default;
  Directory(const Directory&) = delete;
  Directory& operator=(const Directory&) = delete;
  void toString(std::ostream& ostr, unsigned indent_level = 0) const
      noexcept override;
  bool compare(const FileSystemNode& node) const noexcept override;

  void addChild(std::unique_ptr<FileSystemNode> child) noexcept;
  FileSystemNode* getChild(const std::string& name) noexcept;

  static std::unique_ptr<Directory> create(
      std::string name, const std::vector<std::string>& files) noexcept;

  std::string getName() const noexcept override;

private:
  std::string name;
  std::vector<std::string> files;
  Subdirectories sub_directories;
};

std::ostream& operator<<(
    std::ostream& ostr, const Directory& directory) noexcept;

} /* namespace OData */

#endif /* SRC_DIRECTORY_H_ */
