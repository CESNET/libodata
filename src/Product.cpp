#include "Product.h"

#include <ostream>
#include <sstream>

namespace OData {

Product::Product(
    std::string id,
    std::string name,
    std::string ingestion_date,
    std::string filename,
    std::string platform) noexcept : id(std::move(id)),
                                     name(std::move(name)),
                                     ingestion_date(std::move(ingestion_date)),
                                     filename(std::move(filename)),
                                     platform(std::move(platform)),
                                     files() {
}

bool Product::operator==(const Product& entry) const {
  return id == entry.id && name == entry.name
         && ingestion_date == entry.ingestion_date && filename == entry.filename
         && platform == entry.platform && files == entry.files;
}

void Product::setFiles(std::vector<std::string> files) noexcept {
  this->files = std::move(files);
}

std::string Product::getProductPath() const noexcept {
  std::stringstream path;
  path << "Products('" << id << "')/Nodes('" << filename << "')";
  return path.str();
}

void Product::toString(std::ostream& ostr) const noexcept {
  ostr << "{\n\tid=" << id << "\n\tname=" << name
       << "\n\tingestion_date=" << ingestion_date << "\n\tfilename=" << filename
       << "\n\tplatform=" << platform << "\n\tfiles {\n";
  for (const auto& file : files) {
    ostr << "\t\t" << file << "\n";
  }
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

std::ostream& operator<<(std::ostream& ostr, const Product& product) noexcept {
  product.toString(ostr);
  return ostr;
}

} /* namespace OData */
