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

/**
 * Class representing node in virtual file system
 */
class FileSystemNode {
public:
  FileSystemNode() = default;
  virtual ~FileSystemNode() = default;
  FileSystemNode(const FileSystemNode&) = delete;
  FileSystemNode& operator=(const FileSystemNode&) = delete;
  bool operator==(const FileSystemNode& node) const;
  bool operator!=(const FileSystemNode& node) const;

  virtual void toString(
      std::ostream& ostr, unsigned indent_level = 0) const = 0;
  virtual bool compare(const FileSystemNode& node) const noexcept = 0;

  /**
   * @return node name
   */
  virtual std::string getName() const noexcept = 0;

  /**
   * Search for child file
   * @param begin direct child name
   * @param end end of the path
   * @return filesystem node or nullptr if child does not exist
   */
  virtual std::shared_ptr<FileSystemNode> getFile(
      boost::filesystem::path::const_iterator begin,
      boost::filesystem::path::const_iterator end) const noexcept = 0;

  /**
   * List directory content
   * @return
   */
  virtual std::vector<std::string> readDir() const noexcept = 0;

  /**
   * @return if node is directory
   */
  virtual bool isDirectory() const noexcept = 0;

  /**
   * @return file size in bytes
   */
  virtual std::size_t getSize() const noexcept = 0;

  /**
   * Find node direct child.
   * @param name child name
   * @return child or null if not exist
   */
  virtual std::shared_ptr<FileSystemNode> getChild(
      const std::string& name) const noexcept = 0;

  /**
   * Remove node direct child.
   */
  virtual void removeChild(const std::string& child_name) noexcept = 0;

  /**
   * Add child into filesystem node (if node supports it)
   */
  virtual void addChild(std::shared_ptr<FileSystemNode> child) noexcept = 0;

  /**
   * Helper function for toString method. Indents line to specified level.
   * @param ostr destination
   * @param level indent level
   * @return ostr
   */
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
