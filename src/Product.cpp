#include "Product.h"

#include "File.h"
#include "ProductAttribute.h"
#include "RemoteFile.h"
#include <boost/algorithm/string.hpp>
#include <glog/logging.h>
#include <ostream>
#include <sstream>

BOOST_CLASS_EXPORT_IMPLEMENT(OData::Product)

namespace OData {
namespace {

std::size_t parseSize(const std::string& size_property) {
  std::stringstream stream(size_property);
  double size;
  std::string unit;
  stream >> size >> unit;
  if (unit == "GB") {
    size *= 1000 * 1000 * 1000;
  } else if (unit == "MB") {
    size *= 1000 * 1000;
  } else if (unit == "KB") {
    size *= 1000;
  }
  return static_cast<std::size_t>(size);
}

enum DateValue { YEAR = 0, MONTH = 1, DAY = 2 };

} // namespace

Product::Product(std::map<std::string, std::string> attributes) noexcept
    : attributes(std::move(attributes)),
      directory(),
      manifest(),
      archive(std::make_shared<RemoteFile>(
          getRequiredAttribute(Attribute::NAME) + ".zip",
          ProductPath(getRequiredAttribute(Attribute::ID)),
          parseSize(getRequiredAttribute(Attribute::SIZE)))) {
}

void Product::setArchiveStructure(
    std::shared_ptr<Directory> directory,
    std::shared_ptr<File> manifest) noexcept {
  assert(directory != nullptr && manifest != nullptr);
  this->directory = std::move(directory);
  this->manifest = std::move(manifest);
}

ProductPath Product::getProductPath() const noexcept {
  return archive->getProductPath();
}

ProductPath Product::getArchivePath() const noexcept {
  return ProductPath(
      getRequiredAttribute(Attribute::ID),
      getRequiredAttribute(Attribute::FILENAME));
}

void Product::toString(std::ostream& ostr, unsigned indent_level) const {
  indent(ostr, indent_level) << "{\n";
  for (const auto& property : attributes) {
    indent(ostr, indent_level + 1)
        << property.first << "=" << property.second << "\n";
  }
  indent(ostr, indent_level + 1) << "files {\n";
  archive->toString(ostr, indent_level + 2);
  if (directory != nullptr) {
    directory->toString(ostr, indent_level + 2);
  }
  if (manifest != nullptr) {
    manifest->toString(ostr, indent_level + 2);
  }
  indent(ostr, indent_level + 1) << "}\n";
  indent(ostr, indent_level) << "}\n";
}

std::string Product::getPlatform() const noexcept {
  return getRequiredAttribute(Attribute::PLATFORM);
}

std::string Product::getManifestFilename() const noexcept {
  const auto platform = getPlatform();
  if (platform == "Sentinel-1" || platform == "Sentinel-2") {
    return "manifest.safe";
  } else if (platform == "Sentinel-3") {
    return "xfdumanifest.xml";
  } else {
    return "";
  }
}

bool Product::compare(const FileSystemNode& node) const noexcept {
  const auto* entry = dynamic_cast<const Product*>(&node);
  if (entry == nullptr || attributes != entry->attributes
      || *archive != *entry->archive) {
    return false;
  } else if (isArchiveSet() == entry->isArchiveSet()) {
    return !isArchiveSet()
           || ((*directory == *entry->directory)
               && (*manifest == *entry->manifest));
  } else {
    return false;
  }
}

std::string Product::getName() const noexcept {
  return getRequiredAttribute(Attribute::NAME);
}


void Product::setName(std::string) noexcept {
  //nothing to do
}

std::shared_ptr<FileSystemNode> Product::getFile(
    boost::filesystem::path::const_iterator begin,
    boost::filesystem::path::const_iterator end) const noexcept {
  if (begin == end || !isArchiveSet()) {
    return {};
  }
  const auto name = begin->string();
  const auto next = ++begin;
  const bool is_last = next == end;
  const auto child = getChild(name);
  if (child != nullptr) {
    return is_last ? child : child->getFile(next, end);
  }
  return {};
}

std::vector<std::string> Product::readDir() const noexcept {
  if (isArchiveSet()) {
    return {archive->getName(), directory->getName(), manifest->getName()};
  } else {
    return {archive->getName()};
  }
}

bool Product::isDirectory() const noexcept {
  return true;
}

std::size_t Product::getSize() const noexcept {
  return archive->getSize();
}

void Product::removeChild(const std::string&) noexcept {
}

std::string Product::getFilename() const noexcept {
  return archive->getName();
}

std::string Product::getDate() const noexcept {
  const auto creation_date = getRequiredAttribute(Attribute::CREATION_DATE);
  return std::string(creation_date, 0, creation_date.find("T"));
}

std::string Product::getId() const noexcept {
  return getRequiredAttribute(Attribute::ID);
}

boost::optional<std::string> Product::getAttribute(
    const std::string& name) const noexcept {
  if (name == "year") {
    return getDateAttribute(YEAR);
  }
  if (name == "month") {
    return getDateAttribute(MONTH);
  }
  if (name == "day") {
    return getDateAttribute(DAY);
  }
  if (name == "date") {
    return boost::optional<std::string>(getDate());
  }
  const auto it = attributes.find(name);
  if (it == attributes.end()) {
    return boost::optional<std::string>();
  } else {
    return boost::optional<std::string>(it->second);
  }
}

std::ostream& operator<<(std::ostream& ostr, const Product& product) noexcept {
  product.toString(ostr);
  return ostr;
}

bool Product::isArchiveSet() const {
  return manifest != nullptr && directory != nullptr;
}

std::string Product::getRequiredAttribute(const std::string& name) const
    noexcept {
  const auto attribute = getAttribute(name);
  if (attribute) {
    return *attribute;
  } else {
    LOG(WARNING) << "Product attribute '" << name << "' not set.";
    return std::string();
  }
}

boost::optional<std::string> Product::getDateAttribute(unsigned attribute) const
    noexcept {
  std::vector<std::string> split;
  const auto date = getDate();
  boost::split(split, date, [](char c) { return c == '-'; });
  if (split.size() > attribute) {
    return boost::optional<std::string>(split[attribute]);
  } else {
    return boost::optional<std::string>();
  }
}

std::shared_ptr<FileSystemNode> Product::getChild(const std::string& name) const
    noexcept {
  if (isArchiveSet()) {
    if (name == archive->getName()) {
      return archive;
    } else if (name == directory->getName()) {
      return directory;
    } else if (name == manifest->getName()) {
      return manifest;
    }
  }
  return {};
}

void Product::addChild(std::shared_ptr<FileSystemNode>) noexcept {
}

void Product::updateVersion() {
  archive->setName(getRequiredAttribute(Attribute::NAME) + ".zip");
  directory->setName(getRequiredAttribute(Attribute::FILENAME));
}

} /* namespace OData */
