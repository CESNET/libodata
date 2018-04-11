#include "FileSystemNode.h"

#include <list>
#include <ostream>
#include <sstream>

namespace OData {

bool FileSystemNode::operator==(const FileSystemNode& node) const {
  return compare(node);
}

bool FileSystemNode::operator!=(const FileSystemNode& node) const {
  return !compare(node);
}

std::shared_ptr<FileSystemNode> FileSystemNode::getFile(
    const boost::filesystem::path& path) const noexcept {
  if (path.empty()) {
    return {};
  } else {
    return getFile(path.begin(), path.end());
  }
}

std::ostream& FileSystemNode::indent(std::ostream& ostr, unsigned level) {
  for (unsigned i = 0; i < level; ++i) {
    ostr << "\t";
  }
  return ostr;
}

std::ostream& operator<<(
    std::ostream& ostr, const FileSystemNode& node) noexcept {
  node.toString(ostr);
  return ostr;
}

} /* namespace OData */
