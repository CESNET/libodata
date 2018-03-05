#ifndef SRC_FILE_H_
#define SRC_FILE_H_

#include "FileSystemNode.h"
#include <string>
#include <vector>

namespace OData {

class File : public FileSystemNode {
public:
  File(std::string name, std::vector<char> data) noexcept;
  ~File() = default;
  File(const File&) = delete;
  File& operator=(const File&) = delete;

  void toString(std::ostream& ostr, unsigned indent_level = 0) const
      noexcept override;
  bool compare(const FileSystemNode& node) const noexcept override;
  std::string getName() const noexcept;
  const FileSystemNode* getFile(
      boost::filesystem::path::const_iterator begin,
      boost::filesystem::path::const_iterator end) const noexcept override;
  std::vector<std::string> readDir() const noexcept override;
  bool isDirectory() const noexcept override;

  const std::vector<char>& getData() const noexcept;

private:
  std::string name;
  std::vector<char> data;
};

} /* namespace OData */

#endif /* SRC_FILE_H_ */
