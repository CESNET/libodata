#include "Archive.h"

BOOST_CLASS_EXPORT_IMPLEMENT(OData::Archive)

namespace OData {

Archive::Archive(
    std::shared_ptr<Directory> content,
    ProductPath path,
    std::size_t size) noexcept
    : content(std::move(content)), path(std::move(path)), size(size) {
  assert(this->content != nullptr);
}

void Archive::toString(std::ostream& ostr, unsigned indent_level) const
    noexcept {
  content->toString(ostr, indent_level);
}

bool Archive::compare(const FileSystemNode& node) const noexcept {
  const auto* other = dynamic_cast<const Archive*>(&node);
  return other != nullptr && size == other->size && path == other->path
         && content != nullptr && other->content != nullptr
         && *content == *other->content;
}

std::string Archive::getName() const noexcept {
  return content->getName();
}

std::shared_ptr<FileSystemNode> Archive::getFile(
    boost::filesystem::path::const_iterator begin,
    boost::filesystem::path::const_iterator end) const noexcept {
  return content->getFile(begin, end);
}

std::vector<std::string> Archive::readDir() const noexcept {
  return content->readDir();
}

bool Archive::isDirectory() const noexcept {
  return content->isDirectory();
}

std::size_t Archive::getSize() const noexcept {
  return size;
}

ProductPath Archive::getProductPath() const noexcept {
  return path;
}

} /* namespace OData */
