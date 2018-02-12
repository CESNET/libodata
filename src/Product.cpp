#include "Product.h"

#include <algorithm>
#include <ostream>
#include <sstream>

namespace OData {
namespace {
std::string createFilename(
    const std::string& prefix,
    unsigned index,
    std::string type,
    const std::string& timestamp,
    unsigned file_index,
    const std::string& suffix) {
  std::transform(type.begin(), type.end(), type.begin(), ::tolower);
  std::stringstream str;
  str << prefix << index << "-slc-" << type << "-" << timestamp << "00"
      << file_index << "." << suffix;
  return str.str();
}

std::unique_ptr<Directory> createSentinel1SLCDirectory(
    std::string filename, const std::vector<std::string>& polarization_modes) {
  auto archive = std::make_unique<Directory>(filename);
  std::transform(
      filename.begin(), filename.end(), filename.begin(), [](char c) {
        return c == '_' ? '-' : std::tolower(c);
      });
  const auto prefix = filename.substr(0, 6);
  const auto timestamp = filename.substr(17, 46);
  {
    auto annotation = std::make_unique<Directory>("annotation");
    {
      auto calibration = std::make_unique<Directory>("calibration");
      for (auto i = 1u; i <= 3; ++i) {
        for (auto mode_index = 0u; mode_index < polarization_modes.size();
             ++mode_index) {
          calibration->addFile(createFilename(
              "noise-" + prefix,
              i,
              polarization_modes[mode_index],
              timestamp,
              i + mode_index * 3,
              "xml"));
          calibration->addFile(createFilename(
              "calibration-" + prefix,
              i,
              polarization_modes[mode_index],
              timestamp,
              i + mode_index * 3,
              "xml"));
        }
      }
      annotation->addChild(std::move(calibration));
    }
    for (auto i = 1u; i <= 3; ++i) {
      for (auto mode_index = 0u; mode_index < polarization_modes.size();
           ++mode_index) {
        annotation->addFile(createFilename(
            prefix,
            i,
            polarization_modes[mode_index],
            timestamp,
            i + mode_index * 3,
            "xml"));
      }
      archive->addChild(std::move(annotation));
    }
    {
      auto measurement = std::make_unique<Directory>("measurement");
      for (auto i = 1u; i <= 3; ++i) {
        for (auto mode_index = 0u; mode_index < polarization_modes.size();
             ++mode_index) {
          measurement->addFile(createFilename(
              prefix,
              i,
              polarization_modes[mode_index],
              timestamp,
              i + mode_index * 3,
              "tiff"));
        }
      }
      archive->addChild(std::move(measurement));
    }
    archive->addChild(std::make_unique<Directory>(
        "preview",
        std::vector<std::string>{
            "map-overlay.kml", "product-preview.html", "quick-look.png"},
        Directory::Subdirectories{}));
    return archive;
  }
}
} // namespace

Product::Product(
    std::string id,
    std::string name,
    std::string ingestion_date,
    std::string filename,
    std::string platform,
    std::string type,
    std::vector<std::string> polarisation_modes) noexcept
    : id(std::move(id)),
      name(std::move(name)),
      ingestion_date(std::move(ingestion_date)),
      filename(std::move(filename)),
      platform(std::move(platform)),
      type(std::move(type)),
      polarisation_modes(std::move(polarisation_modes)),
      directory() {
  if (this->platform == "Sentinel-1" && this->type == "SLC") {
    this->directory =
        createSentinel1SLCDirectory(this->filename, this->polarisation_modes);
  }
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
  indent(ostr, indent_level + 1) << "type=" << type << "\n";
  indent(ostr, indent_level + 1) << "files {\n";
  if (directory != nullptr) {
    directory->toString(ostr, indent_level + 2);
  }
  indent(ostr, indent_level + 1) << "}\n";
  indent(ostr, indent_level) << "}\n";
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

std::string Product::getName() const noexcept {
  return name;
}

const std::string& Product::getFilename() const noexcept {
  return filename;
}

std::string Product::getDate() const noexcept {
  return std::string(ingestion_date, 0, ingestion_date.find("T"));
}

std::ostream& operator<<(std::ostream& ostr, const Product& product) noexcept {
  product.toString(ostr);
  return ostr;
}

} // namespace OData
