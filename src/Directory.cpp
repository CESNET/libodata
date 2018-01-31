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

std::ostream& indent(std::ostream& ostr, unsigned level) {
  for (unsigned i = 0; i < level; ++i) {
    ostr << "\t";
  }
  return ostr;
}
} // namespace

Directory::Directory(
    std::string name,
    std::vector<std::string> files,
    std::vector<Directory> sub_directories) noexcept
    : name(std::move(name)),
      files(std::move(files)),
      sub_directories(std::move(sub_directories)) {
}

bool Directory::operator==(const Directory& other) const noexcept {
  return name == other.name && files == other.files
         && sub_directories == other.sub_directories;
}

void Directory::toString(std::ostream& ostr, unsigned indent_level) const
    noexcept {
  indent(ostr, indent_level) << name << " {\n";
  for (const auto& sub_dir : sub_directories) {
    sub_dir.toString(ostr, indent_level + 1);
  }
  for (const auto& file : files) {
    indent(ostr, indent_level + 1) << file << "\n";
  }
  indent(ostr, indent_level) << "}\n";
}

Directory Directory::create(
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
  std::vector<Directory> sub_dirs;
  for (const auto& sub_dir : dirs) {
    sub_dirs.emplace_back(Directory::create(sub_dir.first, sub_dir.second));
  }
  return Directory(std::move(name), std::move(content), std::move(sub_dirs));
}

std::ostream& operator<<(
    std::ostream& ostr, const Directory& directory) noexcept {
  directory.toString(ostr);
  return ostr;
}

} /* namespace OData */
