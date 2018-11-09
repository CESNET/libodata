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

Directory* getPlatform(
    Directory::Content& platforms, const std::string& platform) {
  auto platform_tree = platforms.find(platform);
  if (platform_tree == platforms.end()) {
    return nullptr;
  } else {
    return static_cast<Directory*>(platform_tree->second.get());
  }
}

Directory* getOrInsertDateSubtree(
    Directory::Content& platforms,
    const std::string& product_platform,
    const std::string& product_date) {
  Directory* platform = getOrInsertPlatform(platforms, product_platform);
  auto date_tree = dynamic_cast<Directory*>(platform->getChild(product_date));
  if (date_tree == nullptr) {
    auto sub_tree = std::make_shared<Directory>(product_date);
    date_tree = sub_tree.get();
    platform->addChild(sub_tree);
  }
  return date_tree;
}

Directory* getDateSubtree(
    Directory::Content& platforms,
    const std::string& product_platform,
    const std::string& product_date) {
  Directory* platform = getPlatform(platforms, product_platform);
  if (platform == nullptr) {
    return nullptr;
  } else {
    return dynamic_cast<Directory*>(platform->getChild(product_date));
  }
}
} // namespace

Directory::Directory(std::string name, Content content) noexcept
    : name(std::move(name)), content(std::move(content)), content_mutex() {
}

void Directory::toString(std::ostream& ostr, unsigned indent_level) const {
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

std::shared_ptr<FileSystemNode> Directory::getFile(
    boost::filesystem::path::const_iterator begin,
    boost::filesystem::path::const_iterator end) const noexcept {
  boost::shared_lock<boost::shared_mutex> lock(content_mutex);
  if (begin == end) {
    return nullptr;
  }
  auto sub_dir_iter = content.find(begin->string());
  if (sub_dir_iter == content.end()) {
    return {};
  }
  const auto next = ++begin;
  if (next == end) {
    return sub_dir_iter->second;
  } else {
    return sub_dir_iter->second->getFile(next, end);
  }
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

std::size_t Directory::getSize() const noexcept {
  return 0;
}

void Directory::addChild(std::shared_ptr<FileSystemNode> child) noexcept {
  boost::unique_lock<boost::shared_mutex> lock(content_mutex);
  auto name = child->getName();
  content[std::move(name)] = std::move(child);
}

void Directory::removeChild(const std::string& child_name) noexcept {
  boost::unique_lock<boost::shared_mutex> lock(content_mutex);
  auto it = content.find(child_name);
  if (it != content.end()) {
    content.erase(it);
  }
}

std::unique_ptr<Directory> Directory::createRemoteStructure(
    const ProductPath& product_path,
    std::string name,
    const std::vector<std::pair<boost::filesystem::path, std::size_t>>&
        files) noexcept {
  std::map<std::string, std::shared_ptr<FileSystemNode>> dir_content;
  std::map<
      std::string,
      std::vector<std::pair<boost::filesystem::path, std::size_t>>>
      sub_dirs;
  for (const auto& file : files) {
    if (!file.first.empty()) {
      auto filename = file.first.begin()->string();
      if (++file.first.begin() == file.first.end()) {
        dir_content[filename] = std::make_shared<RemoteFile>(
            filename, ProductPath(product_path, filename), file.second);
      } else {
        const auto position = file.first.string().find(filename);
        boost::filesystem::path child_path(
            file.first.string().c_str() + position + filename.size() + 1);
        sub_dirs[std::move(filename)].emplace_back(
            std::make_pair(std::move(child_path), file.second));
      }
    }
  }
  for (const auto& sub_dir : sub_dirs) {
    dir_content[sub_dir.first] = Directory::createRemoteStructure(
        ProductPath(product_path, sub_dir.first),
        sub_dir.first,
        sub_dir.second);
  }
  return std::make_unique<Directory>(std::move(name), std::move(dir_content));
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

void Directory::appendProducts(
    std::vector<std::shared_ptr<Product>> products) noexcept {
  for (auto product : products) {
    appendProduct(product, product->getPlatform(), product->getDate());
  }
}

void Directory::appendProduct(
    std::shared_ptr<FileSystemNode> product,
    const std::string& platform,
    const std::string& date) noexcept {
  boost::unique_lock<boost::shared_mutex> lock(content_mutex);
  auto parent = getOrInsertDateSubtree(content, platform, date);
  parent->addChild(std::move(product));
}

void Directory::removeProduct(
    const std::string& product_name,
    const std::string& platform,
    const std::string& date) noexcept {
  boost::unique_lock<boost::shared_mutex> lock(content_mutex);
  auto parent = getDateSubtree(content, platform, date);
  if (parent != nullptr) {
    parent->removeChild(product_name);
  }
}

} /* namespace OData */
