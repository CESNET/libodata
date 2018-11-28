#ifndef ODATA_PATHSERIALIZER_H
#define ODATA_PATHSERIALIZER_H

#include <boost/filesystem/path.hpp>

namespace boost {
namespace serialization {
class access;
} // namespace serialization
} // namespace boost

namespace OData {

class PathSerializer {
public:
  PathSerializer(boost::filesystem::path& path) : path(path) {
  }

private:
  friend class boost::serialization::access;
  template <typename Archive> void serialize(Archive& ar, const unsigned int) {
    std::string raw = path.string();
    ar& raw;
    path = boost::filesystem::path(raw);
  }

  boost::filesystem::path& path;
};

} // namespace OData

#endif // ODATA_PATHSERIALIZER_H
