#ifndef SRC_REMOTEFILE_H_
#define SRC_REMOTEFILE_H_

#include "FileSystemNode.h"
#include "ProductPath.h"
#include <string>

namespace OData {

class RemoteFile : public FileSystemNode {
public:
  RemoteFile(std::string name, ProductPath path) noexcept;
  virtual ~RemoteFile() = default;
  RemoteFile(const RemoteFile&) = delete;
  RemoteFile& operator=(const RemoteFile&) = delete;

  void toString(std::ostream& ostr, unsigned indent_level = 0) const
      noexcept override;
  bool compare(const FileSystemNode& node) const noexcept override;
  std::string getName() const noexcept;
  const FileSystemNode* getFile(
      boost::filesystem::path::const_iterator begin,
      boost::filesystem::path::const_iterator end) const noexcept override;
  std::vector<std::string> readDir() const noexcept override;

private:
  std::string name;
  ProductPath path;
};

} /* namespace OData */

#endif /* SRC_REMOTEFILE_H_ */
