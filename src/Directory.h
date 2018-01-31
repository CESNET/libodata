#ifndef SRC_DIRECTORY_H_
#define SRC_DIRECTORY_H_

#include <iosfwd>
#include <string>
#include <vector>

namespace OData {

class Directory {
public:
  Directory() = default;
  Directory(
      std::string name,
      std::vector<std::string> files,
      std::vector<Directory> sub_directories) noexcept;
  ~Directory() = default;
  Directory(const Directory&) = delete;
  Directory(Directory&&) = default;
  Directory& operator=(const Directory&) = delete;
  Directory& operator=(Directory&&) = default;
  bool operator==(const Directory& other) const noexcept;
  void toString(std::ostream& ostr, unsigned indent_level = 0) const noexcept;

  static Directory create(
      std::string name, const std::vector<std::string>& files) noexcept;

private:
  std::string name;
  std::vector<std::string> files;
  std::vector<Directory> sub_directories;
};

std::ostream& operator<<(
    std::ostream& ostr, const Directory& directory) noexcept;

} /* namespace OData */

#endif /* SRC_DIRECTORY_H_ */
