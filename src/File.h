#ifndef SRC_FILE_H_
#define SRC_FILE_H_

#include "FileSystemNode.h"
#include <string>

namespace OData {

class File : public FileSystemNode {
public:
  explicit File(std::string name) noexcept;
  ~File() = default;
  File(const File&) = delete;
  File& operator=(const File&) = delete;

  void toString(std::ostream& ostr, unsigned indent_level = 0) const
      noexcept override;
  bool compare(const FileSystemNode& node) const noexcept override;
  std::string getName() const noexcept;
  FileSystemNode* getFile(std::list<std::string> path) const noexcept override;
  std::vector<std::string> readDir() const noexcept override;

private:
  std::string name;
};

} /* namespace OData */

#endif /* SRC_FILE_H_ */
