#include "RemoteFile.h"

BOOST_CLASS_EXPORT_IMPLEMENT(OData::RemoteFile)

namespace OData {

RemoteFile::RemoteFile(
    std::string name, ProductPath path, std::size_t size) noexcept
    : name(std::move(name)), path(std::move(path)), size(size) {
}

void RemoteFile::toString(std::ostream& ostr, unsigned indent_level) const
    noexcept {
  indent(ostr, indent_level) << name << "( " << size << " )"
                             << "^" << path.getPath() << "\n";
}

bool RemoteFile::compare(const FileSystemNode& node) const noexcept {
  const auto other = dynamic_cast<const RemoteFile*>(&node);
  return other != nullptr && other->name == name && other->path == path
         && other->size == size;
}

std::string RemoteFile::getName() const noexcept {
  return name;
}

std::shared_ptr<FileSystemNode> RemoteFile::getFile(
    boost::filesystem::path::const_iterator,
    boost::filesystem::path::const_iterator) const noexcept {
  return nullptr;
}

std::vector<std::string> RemoteFile::readDir() const noexcept {
  return {};
}

bool RemoteFile::isDirectory() const noexcept {
  return false;
}

std::size_t RemoteFile::getSize() const noexcept {
  return size;
}

ProductPath RemoteFile::getProductPath() const noexcept {
  return path;
}

} /* namespace OData */
