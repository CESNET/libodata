#include "File.h"

#include <ostream>

BOOST_CLASS_EXPORT_IMPLEMENT(OData::File)

namespace OData {

File::File(std::string name, std::vector<char> data) noexcept
    : name(std::move(name)), data(std::move(data)) {
}

void File::toString(std::ostream& ostr, unsigned indent_level) const {
  indent(ostr, indent_level) << name << "\n";
}

bool File::compare(const FileSystemNode& node) const noexcept {
  const auto* other = dynamic_cast<const File*>(&node);
  return other != nullptr && name == other->name && data == other->data;
}

std::string File::getName() const noexcept {
  return name;
}

void File::setName(std::string name) noexcept {
  this->name = std::move(name);
}

std::shared_ptr<FileSystemNode> File::getFile(
    boost::filesystem::path::const_iterator,
    boost::filesystem::path::const_iterator) const noexcept {
  return nullptr;
}

std::vector<std::string> File::readDir() const noexcept {
  return {};
}

bool File::isDirectory() const noexcept {
  return false;
}

std::size_t File::getSize() const noexcept {
  return data.size();
}

const std::vector<char>& File::getData() const noexcept {
  return data;
}

std::shared_ptr<FileSystemNode> File::getChild(const std::string&) const
    noexcept {
  return {};
}

void File::removeChild(const std::string&) noexcept {
}

void File::addChild(std::shared_ptr<FileSystemNode>) noexcept {
}

} /* namespace OData */
