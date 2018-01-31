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

bool Product::operator==(const Product& entry) const {
  return id == entry.id && name == entry.name
         && ingestion_date == entry.ingestion_date && filename == entry.filename
         && platform == entry.platform && directory == entry.directory;
}

void Product::setArchiveStructure(Directory directory) noexcept {
  this->directory = std::move(directory);
}

ProductPath Product::getProductPath() const noexcept {
  return ProductPath(id, filename);
}

void Product::toString(std::ostream& ostr) const noexcept {
  ostr << "{\n\tid=" << id << "\n\tname=" << name
       << "\n\tingestion_date=" << ingestion_date << "\n\tfilename=" << filename
       << "\n\tplatform=" << platform << "\n\tfiles {\n";
  directory.toString(ostr, 2);
  ostr << "\t}\n}";
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

const std::string& Product::getFilename() const {
  return filename;
}

std::ostream& operator<<(std::ostream& ostr, const Product& product) noexcept {
  product.toString(ostr);
  return ostr;
}

} /* namespace OData */
