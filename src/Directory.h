#ifndef SRC_DIRECTORY_H_
#define SRC_DIRECTORY_H_

#include "FileSystemNode.h"
#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

namespace OData {

class Directory : public FileSystemNode {
public:
  Directory(
      std::string name,
      std::vector<std::string> files,
      std::vector<std::unique_ptr<FileSystemNode>> sub_directories) noexcept;
  virtual ~Directory() = default;
  Directory(const Directory&) = delete;
  Directory& operator=(const Directory&) = delete;
  void toString(std::ostream& ostr, unsigned indent_level = 0) const
      noexcept override;
  bool compare(const FileSystemNode& node) const noexcept override;

  static std::unique_ptr<Directory> create(
      std::string name, const std::vector<std::string>& files) noexcept;

private:
  std::string name;
  std::vector<std::string> files;
  std::vector<std::unique_ptr<FileSystemNode>> sub_directories;
};

std::ostream& operator<<(
    std::ostream& ostr, const Directory& directory) noexcept;

} /* namespace OData */

#endif /* SRC_DIRECTORY_H_ */
