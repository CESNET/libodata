#include "TemporaryFile.h"

#include <cstdio>
#include <fstream>

namespace OData {

OData::TemporaryFile::TemporaryFile(
    ProductPath file_name, boost::filesystem::path path) noexcept
    : file_name(std::move(file_name)), path(std::move(path)) {
}

TemporaryFile::~TemporaryFile() {
  std::remove(path.c_str());
}

std::vector<char> TemporaryFile::read(off64_t offset, std::size_t length) {
  std::fstream file(path.c_str(), std::ios_base::in);
  file.seekg(offset);
  std::vector<char> buffer(length);
  file.read(buffer.data(), length);
  buffer.resize(file.gcount());
  return buffer;
}

const ProductPath& TemporaryFile::getFileName() const noexcept {
  return file_name;
}

} /* namespace OData */
