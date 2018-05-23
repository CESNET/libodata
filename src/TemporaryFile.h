#ifndef SRC_TEMPORARYFILE_H_
#define SRC_TEMPORARYFILE_H_

#include "ProductPath.h"
#include <boost/filesystem/path.hpp>
#include <cstdint>
#include <string>
#include <unistd.h>
#include <vector>

namespace OData {

class TemporaryFile {
public:
  TemporaryFile(ProductPath file_name, boost::filesystem::path path) noexcept;
  TemporaryFile(const TemporaryFile&) = delete;
  ~TemporaryFile();
  TemporaryFile& operator=(const TemporaryFile&) = delete;
  std::vector<char> read(off64_t offset, std::size_t length);
  const ProductPath& getFileName() const noexcept;

private:
  ProductPath file_name;
  boost::filesystem::path path;
};

} /* namespace OData */

#endif /* SRC_TEMPORARYFILE_H_ */
