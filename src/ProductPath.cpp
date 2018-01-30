#include "ProductPath.h"

#include <iterator>
#include <sstream>
#include <utility>

namespace OData {
ProductPath::ProductPath(std::string uuid, std::string filename) noexcept
    : ProductPath(std::move(uuid), std::move(filename), {}) {
}

ProductPath::ProductPath(
    std::string uuid,
    std::string filename,
    std::initializer_list<std::string> path) noexcept
    : uuid(std::move(uuid)),
      filename(std::move(filename)),
      path(std::move(path)) {
}

void ProductPath::appendPath(std::initializer_list<std::string> path) noexcept {
  std::move(path.begin(), path.end(), std::back_inserter(this->path));
}

std::string ProductPath::getPath() const noexcept {
  std::stringstream uri;
  uri << "Products('" << uuid << "')/Nodes('" << filename << "')";
  for (const auto& item : path) {
    uri << "/Nodes('" << item << "')";
  }
  uri << "/$value";
  return uri.str();
}

} /* namespace OData */
