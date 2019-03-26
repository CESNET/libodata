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
  TemporaryFile() = default;
  TemporaryFile(const TemporaryFile&) = delete;
  virtual ~TemporaryFile() = default;
  TemporaryFile& operator=(const TemporaryFile&) = delete;

  /**
   * Read file content
   * @param offset position of first byte to read
   * @param length how many bytes to read
   * @return up to length bytes read from file or empty vector if offset is out
   * of bounds
   */
  virtual std::vector<char> read(
      off64_t offset, std::size_t length) noexcept = 0;
};

/**
 * File cached on local filesystem. File is deleted on object destruction.
 */
class TemporaryFileImpl : public TemporaryFile {
public:
  TemporaryFileImpl(
      ProductPath file_name, boost::filesystem::path path) noexcept;
  TemporaryFileImpl(const TemporaryFileImpl&) = delete;
  ~TemporaryFileImpl();
  TemporaryFileImpl& operator=(const TemporaryFile&) = delete;
  std::vector<char> read(off64_t offset, std::size_t length) noexcept override;

private:
  ProductPath file_name;
  boost::filesystem::path path;
};

} /* namespace OData */

#endif /* SRC_TEMPORARYFILE_H_ */
