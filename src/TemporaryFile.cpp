#include "TemporaryFile.h"

#include <algorithm>
#include <cstdio>
#include <fstream>

#include <iostream>

namespace OData {

OData::TemporaryFile::TemporaryFile(
    ProductPath file_name, boost::filesystem::path path) noexcept
    : file_name(std::move(file_name)), path(std::move(path)) {
}

TemporaryFile::~TemporaryFile() {
  std::remove(path.c_str());
}

std::vector<char> TemporaryFile::read(off64_t offset, std::size_t length) {
  std::ifstream file(path.c_str(), std::ifstream::binary | std::ifstream::ate);
  file.seekg(0, file.end);
  const auto file_size = static_cast<off64_t>(file.tellg());
  if (file_size <= offset || length == 0) {
    return {};
  }
  const auto read_size =
      std::min(static_cast<std::size_t>(file_size - offset), length);
  file.seekg(offset);
  std::vector<char> buffer(read_size, 0);
  file.read(buffer.data(), read_size);
  return buffer;
}

const ProductPath& TemporaryFile::getFileName() const noexcept {
  return file_name;
}

} /* namespace OData */
