#ifndef ODATA_SERIALIZATIONHELPERS_H
#define ODATA_SERIALIZATIONHELPERS_H

#include <boost/filesystem/path.hpp>
#include <string>

namespace boost {
namespace filesystem {

template <class Archive>
void serialize(Archive& ar, boost::filesystem::path& path, const unsigned int) {
  std::string raw = path.string();
  ar& raw;
  path = boost::filesystem::path(raw);
}

} // namespace filesystem
} // namespace boost

#endif // ODATA_SERIALIZATIONHELPERS_H
