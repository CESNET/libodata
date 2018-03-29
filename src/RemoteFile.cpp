#include "RemoteFile.h"

BOOST_CLASS_EXPORT_IMPLEMENT(OData::RemoteFile)

namespace OData {

RemoteFile::RemoteFile(std::string name, ProductPath path) noexcept
    : name(std::move(name)), path(std::move(path)) {
}

void RemoteFile::toString(std::ostream& ostr, unsigned indent_level) const
    noexcept {
  indent(ostr, indent_level) << name << "^" << path.getPath() << "\n";
}

bool RemoteFile::compare(const FileSystemNode& node) const noexcept {
  const auto other = dynamic_cast<const RemoteFile*>(&node);
  return other != nullptr && other->name == name && other->path == path;
}

std::string RemoteFile::getName() const noexcept {
  return name;
}

const FileSystemNode* RemoteFile::getFile(
    boost::filesystem::path::const_iterator begin,
    boost::filesystem::path::const_iterator end) const noexcept {
  if (begin != end && begin->string() == name && ++begin == end) {
    return this;
  } else {
    return nullptr;
  }
}

std::vector<std::string> RemoteFile::readDir() const noexcept {
  return {};
}

bool RemoteFile::isDirectory() const noexcept {
  return false;
}

ProductPath RemoteFile::getProductPath() const noexcept {
  return ProductPath(path, name);
}

} /* namespace OData */
