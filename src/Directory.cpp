#include "Directory.h"

#include "File.h"
#include "Product.h"
#include "ProductPath.h"
#include "RemoteFile.h"
#include <algorithm>
#include <boost/filesystem/operations.hpp>
#include <map>
#include <ostream>
#include <string>

BOOST_CLASS_EXPORT_IMPLEMENT(OData::Directory)

namespace OData {
namespace {
Directory* getOrInsertPlatform(
    Directory::Content& platforms, const std::string& platform) {
  auto platform_tree = platforms.find(platform);
  if (platform_tree == platforms.end()) {
    platforms[platform] = std::make_shared<Directory>(platform);
    platform_tree = platforms.find(platform);
  }
  return static_cast<Directory*>(platform_tree->second.get());
}

Directory* getOrInsertDateSubtree(
    Directory::Content& platforms, const Product& product) {
  Directory* platform = getOrInsertPlatform(platforms, product.getPlatform());
  const auto date = product.getDate();
  auto date_tree = dynamic_cast<Directory*>(platform->getChild(date));
  if (date_tree == nullptr) {
    auto sub_tree = std::make_shared<Directory>(date);
    date_tree = sub_tree.get();
    platform->addChild(sub_tree);
  }
  return date_tree;
}
} // namespace

Directory::Directory(std::string name, Content content) noexcept
    : name(std::move(name)), content(std::move(content)), content_mutex() {
}

void Directory::toString(std::ostream& ostr, unsigned indent_level) const
    noexcept {
  boost::shared_lock<boost::shared_mutex> lock(content_mutex);
  indent(ostr, indent_level) << name << " {\n";
  for (const auto& sub_dir : content) {
    sub_dir.second->toString(ostr, indent_level + 1);
  }
  indent(ostr, indent_level) << "}\n";
}

bool Directory::compare(const FileSystemNode& node) const noexcept {
  boost::shared_lock<boost::shared_mutex> lock(content_mutex);
  const auto* other = dynamic_cast<const Directory*>(&node);
  if (other == nullptr || name != other->name) {
    return false;
  }
  if (content.size() == other->content.size()) {
    for (auto pair1 = content.cbegin(), pair2 = other->content.cbegin();
         pair1 != content.end();
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

std::string Directory::getName() const noexcept {
  return name;
}

const FileSystemNode* Directory::getFile(
    boost::filesystem::path::const_iterator begin,
    boost::filesystem::path::const_iterator end) const noexcept {
  boost::shared_lock<boost::shared_mutex> lock(content_mutex);
  if (begin == end || begin->string() != name) {
    return nullptr;
  }
  const auto next = ++begin;
  if (next == end) {
    return this;
  }
  auto sub_dir_iter = content.find(next->string());
  auto subdir =
      sub_dir_iter == content.end() ? nullptr : sub_dir_iter->second.get();
  return subdir == nullptr ? nullptr : subdir->getFile(next, end);
}

std::vector<std::string> OData::Directory::readDir() const noexcept {
  boost::shared_lock<boost::shared_mutex> lock(content_mutex);
  std::vector<std::string> children;
  for (const auto& pair : content) {
    children.push_back(pair.first);
  }
  return children;
}

bool Directory::isDirectory() const noexcept {
  return true;
}

void Directory::addChild(std::shared_ptr<FileSystemNode> child) noexcept {
  boost::unique_lock<boost::shared_mutex> lock(content_mutex);
  const auto name = child->getName();
  content[name] = std::move(child);
}

std::unique_ptr<Directory> Directory::createRemoteStructure(
    const ProductPath& product_path,
    std::string name,
    const std::vector<boost::filesystem::path>& files) noexcept {
  std::map<std::string, std::shared_ptr<FileSystemNode>> dir_content;
  std::map<std::string, std::vector<boost::filesystem::path>> sub_dirs;
  for (const auto& file : files) {
    if (!file.empty()) {
      auto filename = file.begin()->string();
      if (++file.begin() == file.end()) {
        dir_content[filename] =
            std::make_shared<RemoteFile>(filename, product_path);
      } else {
        const auto position = file.string().find(filename);
        boost::filesystem::path child_path(
            file.string().c_str() + position + filename.size() + 1);
        sub_dirs[std::move(filename)].emplace_back(std::move(child_path));
      }
    }
  }
  for (const auto& sub_dir : sub_dirs) {
    dir_content[sub_dir.first] = Directory::createRemoteStructure(
        ProductPath(product_path, sub_dir.first),
        sub_dir.first,
        sub_dir.second);
  }
  return std::unique_ptr<Directory>(
      new Directory(std::move(name), std::move(dir_content)));
}

FileSystemNode* Directory::getChild(const std::string& name) noexcept {
  boost::shared_lock<boost::shared_mutex> lock(content_mutex);
  const auto it = content.find(name);
  if (it == content.end()) {
    return nullptr;
  } else {
    return it->second.get();
  }
}

std::ostream& operator<<(
    std::ostream& ostr, const Directory& directory) noexcept {
  directory.toString(ostr);
  return ostr;
}

void Directory::appendProducts(std::vector<std::shared_ptr<Product>> products) {
  boost::unique_lock<boost::shared_mutex> lock(content_mutex);
  for (auto& product : products) {
    auto parent = getOrInsertDateSubtree(content, *product);
    parent->addChild(std::move(product));
  }
}
} /* namespace OData */
