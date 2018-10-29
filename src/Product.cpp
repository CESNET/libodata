#include "Product.h"

#include "File.h"
#include "RemoteFile.h"
#include <glog/logging.h>
#include <ostream>
#include <sstream>

BOOST_CLASS_EXPORT_IMPLEMENT(OData::Product)

namespace OData {
namespace {
// attribute names used by datahub server
constexpr const char* ATTRIBUTE_ID = "uuid";
constexpr const char* ATTRIBUTE_FILENAME = "filename";
constexpr const char* ATTRIBUTE_SIZE = "size";
constexpr const char* ATTRIBUTE_PLATFORM = "platformname";
constexpr const char* ATTRIBUTE_NAME = "identifier";
constexpr const char* ATTRIBUTE_CREATION_DATE = "beginposition";

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
} // namespace

Product::Product(std::map<std::string, std::string> attributes) noexcept
    : attributes(std::move(attributes)),
      directory(),
      manifest(),
      archive(std::make_shared<RemoteFile>(
          this->attributes[ATTRIBUTE_FILENAME],
          ProductPath(this->attributes[ATTRIBUTE_ID]),
          parseSize(this->attributes[ATTRIBUTE_SIZE]))) {
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
  return ProductPath(getRequiredAttribute(ATTRIBUTE_ID), archive->getName());
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
  return getRequiredAttribute(ATTRIBUTE_PLATFORM);
}

std::string Product::getManifestFilename() const noexcept {
  const auto platform = getRequiredAttribute(ATTRIBUTE_PLATFORM);
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
  return getRequiredAttribute(ATTRIBUTE_NAME);
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
  if (name == archive->getName()) {
    return is_last ? archive : archive->getFile(next, end);
  } else if (name == directory->getName()) {
    return is_last ? directory : directory->getFile(next, end);
  } else if (name == manifest->getName()) {
    return is_last ? manifest : manifest->getFile(next, end);
  } else {
    return {};
  }
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

std::string Product::getFilename() const noexcept {
  return archive->getName();
}

std::string Product::getDate() const noexcept {
  const auto creation_date = getRequiredAttribute(ATTRIBUTE_CREATION_DATE);
  return std::string(creation_date, 0, creation_date.find("T"));
}

std::string Product::getId() const noexcept {
  return getRequiredAttribute(ATTRIBUTE_ID);
}

boost::optional<std::string> Product::getAttribute(
    const std::string& name) const noexcept {
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

} /* namespace OData */
