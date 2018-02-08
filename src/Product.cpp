#include "Product.h"

#include <ostream>
#include <sstream>

namespace OData {

Product::Product(
    std::string id,
    std::string name,
    std::string ingestion_date,
    std::string filename,
    std::string platform) noexcept
    : id(std::move(id)),
      name(std::move(name)),
      ingestion_date(std::move(ingestion_date)),
      filename(std::move(filename)),
      platform(std::move(platform)),
      directory() {
}

void Product::setArchiveStructure(
    std::unique_ptr<Directory> directory) noexcept {
  this->directory = std::move(directory);
}

ProductPath Product::getProductPath() const noexcept {
  return ProductPath(id, filename);
}

void Product::toString(std::ostream& ostr, unsigned indent_level) const
    noexcept {
  indent(ostr, indent_level) << "{\n";
  indent(ostr, indent_level + 1) << "id=" << id << "\n";
  indent(ostr, indent_level + 1) << "name=" << name << "\n";
  indent(ostr, indent_level + 1) << "ingestion_date=" << ingestion_date << "\n";
  indent(ostr, indent_level + 1) << "filename=" << filename << "\n";
  indent(ostr, indent_level + 1) << "platform=" << platform << "\n";
  indent(ostr, indent_level + 1) << "files {\n";
  directory->toString(ostr, indent_level + 2);
  indent(ostr, indent_level + 1) << "}\n";
  indent(ostr, indent_level) << "}";
}

const std::string& Product::getPlatform() const noexcept {
  return platform;
}

std::string Product::getManifestFilename() const noexcept {
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
  if (entry == nullptr || id != entry->id || name != entry->name
      || ingestion_date != entry->ingestion_date || filename != entry->filename
      || platform != entry->platform) {
    return false;
  } else {
    if (directory != nullptr && entry->directory != nullptr) {
      return *directory == *entry->directory;
    } else {
      return entry->directory == directory;
    }
  }
}

const std::string& Product::getFilename() const {
  return filename;
}

std::ostream& operator<<(std::ostream& ostr, const Product& product) noexcept {
  product.toString(ostr);
  return ostr;
}

} /* namespace OData */
