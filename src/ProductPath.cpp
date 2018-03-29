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
    boost::filesystem::path path) noexcept
    : uuid(std::move(uuid)),
      filename(std::move(filename)),
      path(std::move(path)) {
}

ProductPath::ProductPath(
    const ProductPath& path, std::string appended_path) noexcept
    : ProductPath(path) {
  append(appended_path);
}

void ProductPath::append(std::string path) noexcept {
  this->path /= path;
}

std::string ProductPath::getPath() const noexcept {
  std::stringstream uri;
  uri << "Products('" << uuid << "')";
  if (!filename.empty()) {
    uri << "/Nodes('" << filename << "')";
  }
  for (const auto& item : path) {
    uri << "/Nodes('" << item.string() << "')";
  }
  uri << "/$value";
  return uri.str();
}

bool ProductPath::operator==(const ProductPath& other) const noexcept {
  return uuid == other.uuid && filename == other.filename && path == other.path;
}

} /* namespace OData */
