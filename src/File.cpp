#include "File.h"

#include <ostream>

namespace OData {

File::File(std::string name, std::vector<char> data) noexcept
    : name(std::move(name)), data(std::move(data)) {
}

void File::toString(std::ostream& ostr, unsigned indent_level) const noexcept {
  indent(ostr, indent_level) << name << "\n";
}

bool File::compare(const FileSystemNode& node) const noexcept {
  const auto* other = dynamic_cast<const File*>(&node);
  return other != nullptr && name == other->name && data == other->data;
}

std::string File::getName() const noexcept {
  return name;
}

const FileSystemNode* File::getFile(
    boost::filesystem::path::const_iterator begin,
    boost::filesystem::path::const_iterator end) const noexcept {
  if (begin != end && begin->string() == name && ++begin == end) {
    return this;
  } else {
    return nullptr;
  }
}

std::vector<std::string> File::readDir() const noexcept {
  return {};
}

bool File::isDirectory() const noexcept {
  return false;
}

const std::vector<char>& File::getData() const noexcept {
  return data;
}

} /* namespace OData */
