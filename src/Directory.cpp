#include "Directory.h"

#include <map>
#include <ostream>

namespace OData {
namespace {
std::pair<std::string, std::string> splitPath(const std::string& path) {
  if (path.length() >= 2 && path[0] == '.' && path[1] == '/') {
    // archive root
    return splitPath(path.substr(2));
  }
  const auto separator_position = path.find('/');
  if (separator_position == std::string::npos) {
    // file
    return std::make_pair(std::string(), path);
  } else {
    // sub directory
    return std::make_pair(
        path.substr(0, separator_position),
        path.substr(separator_position + 1));
  }
}
} // namespace

Directory::Directory(std::string name) : Directory(std::move(name), {}) {
}

Directory::Directory(
    std::string name,
    std::vector<std::string> files,
    Subdirectories sub_directories) noexcept
    : name(std::move(name)),
      files(std::move(files)),
      sub_directories(std::move(sub_directories)) {
}

void Directory::toString(std::ostream& ostr, unsigned indent_level) const
    noexcept {
  indent(ostr, indent_level) << name << " {\n";
  for (const auto& sub_dir : sub_directories) {
    sub_dir.second->toString(ostr, indent_level + 1);
  }
  for (const auto& file : files) {
    indent(ostr, indent_level + 1) << file << "\n";
  }
  indent(ostr, indent_level) << "}\n";
}

bool Directory::compare(const FileSystemNode& node) const noexcept {
  const auto* other = dynamic_cast<const Directory*>(&node);
  if (other == nullptr || name != other->name || files != other->files) {
    return false;
  }
  if (sub_directories.size() == other->sub_directories.size()) {
    for (auto pair1 = sub_directories.cbegin(),
              pair2 = other->sub_directories.cbegin();
         pair1 != sub_directories.end();
         ++pair1, ++pair2) {
      if (*pair1->second != *pair2->second) {
        return false;
      }
    }
    return true;
  } else {
    return false;
  }
}

void Directory::addChild(std::unique_ptr<FileSystemNode> child) noexcept {
  const auto name = child->getName();
  sub_directories[name] = std::move(child);
}

std::unique_ptr<Directory> Directory::create(
    std::string name, const std::vector<std::string>& files) noexcept {
  std::map<std::string, std::vector<std::string>> dirs;
  std::vector<std::string> content;
  for (const auto& file : files) {
    if (!file.empty()) {
      const auto splitted = splitPath(file);
      if (splitted.first.empty()) {
        content.push_back(splitted.second);
      } else {
        dirs[splitted.first].push_back(splitted.second);
      }
    }
  }
  std::map<std::string, std::unique_ptr<FileSystemNode>> sub_dirs;
  for (const auto& sub_dir : dirs) {
    sub_dirs.insert(
        {sub_dir.first, Directory::create(sub_dir.first, sub_dir.second)});
  }
  return std::make_unique<Directory>(
      std::move(name), std::move(content), std::move(sub_dirs));
}

FileSystemNode* Directory::getChild(const std::string& name) noexcept {
  const auto it = sub_directories.find(name);
  if (it == sub_directories.end()) {
    return nullptr;
  } else {
    return it->second.get();
  }
}

std::string Directory::getName() const noexcept {
  return name;
}

std::ostream& operator<<(
    std::ostream& ostr, const Directory& directory) noexcept {
  directory.toString(ostr);
  return ostr;
}

} /* namespace OData */
