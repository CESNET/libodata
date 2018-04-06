#ifndef ODATA_FILESYSTEMNODE_H_
#define ODATA_FILESYSTEMNODE_H_

#include <boost/filesystem/path.hpp>
#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

namespace boost {
namespace serialization {
class access;
} // namespace serialization
} // namespace boost

namespace OData {

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
  // TODO not save with caching in place
  virtual const FileSystemNode* getFile(
      boost::filesystem::path::const_iterator begin,
      boost::filesystem::path::const_iterator end) const noexcept = 0;
  virtual std::vector<std::string> readDir() const noexcept = 0;
  virtual bool isDirectory() const noexcept = 0;

  const FileSystemNode* getFile(const boost::filesystem::path& path) const
      noexcept;
  static std::ostream& indent(std::ostream& ostr, unsigned level);

private:
  friend class boost::serialization::access;
  template <typename Archive> void serialize(Archive&, const unsigned int) {
  }
};

std::ostream& operator<<(
    std::ostream& ostr, const FileSystemNode& node) noexcept;

} /* namespace OData */

#endif /* ODATA_FILESYSTEMNODE_H_ */
