#include "ProductPath.h"

#include <boost/serialization/export.hpp>
#include <sstream>
#include <utility>

BOOST_CLASS_EXPORT(OData::ProductPath)

namespace OData {
ProductPath::ProductPath(std::string uuid) noexcept
    : ProductPath(std::move(uuid), std::string(), {}) {
}

ProductPath::ProductPath(std::string uuid, std::string filename) noexcept
    : ProductPath(std::move(uuid), std::move(filename), {}) {
}

ProductPath::ProductPath(
    std::string uuid,
    std::string filename,
    boost::filesystem::path file_path) noexcept
    : uuid(std::move(uuid)),
      filename(std::move(filename)),
      file_path(std::move(file_path)) {
}

ProductPath::ProductPath(
    const ProductPath& path, std::string appended_path) noexcept
    : ProductPath(path) {
  append(appended_path);
}

void ProductPath::append(std::string path) noexcept {
  this->file_path /= std::move(path);
}

std::string ProductPath::getPath() const noexcept {
  std::stringstream uri;
  uri << "Products('" << uuid << "')";
  if (!filename.empty()) {
    uri << "/Nodes('" << filename << "')";
  }
  for (const auto& item : file_path) {
    uri << "/Nodes('" << item.string() << "')";
  }
  uri << "/$value";
  return uri.str();
}

bool ProductPath::operator==(const ProductPath& other) const noexcept {
  return uuid == other.uuid && filename == other.filename && file_path == other.file_path;
}

bool OData::ProductPath::operator<(const ProductPath& other) const noexcept {
  return getPath() < other.getPath();
}

} /* namespace OData */
