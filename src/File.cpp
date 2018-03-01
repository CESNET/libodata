#include "File.h"

#include <ostream>

namespace OData {

File::File(std::string name) noexcept : name(std::move(name)) {
}

void File::toString(std::ostream& ostr, unsigned indent_level) const noexcept {
  indent(ostr, indent_level) << name << "\n";
}

bool File::compare(const FileSystemNode& node) const noexcept {
  const auto* other = dynamic_cast<const File*>(&node);
  return other != nullptr && name == other->name;
}

std::string File::getName() const noexcept {
  return name;
}

FileSystemNode* File::getFile(std::list<std::string>) const noexcept {
  return nullptr;
}

std::vector<std::string> File::readDir() const noexcept {
  return {};
}

} /* namespace OData */
