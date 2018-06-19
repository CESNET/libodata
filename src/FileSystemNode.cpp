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
