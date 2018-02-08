#include "FileSystemNode.h"

#include "Directory.h"
#include "Product.h"
#include <map>
#include <ostream>
#include <string>

namespace OData {
namespace {
Directory* getOrInsertPlatform(
    Directory::Subdirectories& platforms, const std::string& platform) {
  auto platform_tree = platforms.find(platform);
  if (platform_tree == platforms.end()) {
    platform_tree =
        platforms.insert({platform, std::make_unique<Directory>(platform)})
            .first;
  }
  return static_cast<Directory*>(platform_tree->second.get());
}

Directory* getOrInsertDateSubtree(
    Directory::Subdirectories& platforms, const Product& product) {
  Directory* platform = getOrInsertPlatform(platforms, product.getPlatform());
  const auto date = product.getDate();
  auto date_tree = dynamic_cast<Directory*>(platform->getChild(date));
  if (date_tree == nullptr) {
    auto sub_tree = std::make_unique<Directory>(date);
    date_tree = sub_tree.get();
    platform->addChild(std::move(sub_tree));
  }
  return date_tree;
}
} // namespace

bool FileSystemNode::operator==(const FileSystemNode& node) const {
  return compare(node);
}

bool FileSystemNode::operator!=(const FileSystemNode& node) const {
  return !compare(node);
}

std::ostream& OData::FileSystemNode::indent(
    std::ostream& ostr, unsigned level) {
  for (unsigned i = 0; i < level; ++i) {
    ostr << "\t";
  }
  return ostr;
}

std::unique_ptr<FileSystemNode> FileSystemNode::create(
    std::vector<std::unique_ptr<Product>> products) noexcept {
  Directory::Subdirectories missions;
  for (auto& product : products) {
    auto parent = getOrInsertDateSubtree(missions, *product);
    parent->addChild(std::move(product));
  }
  return std::make_unique<Directory>(
      "root", std::vector<std::string>{}, std::move(missions));
}

} /* namespace OData */
