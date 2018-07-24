#include "FuseAdapter.h"

#include "Config.h"
#include "FileSystemNode.h"
#include <cstring>
#include <functional>
#include <map>
#include <string>
#include <sys/stat.h>

extern "C" {

static int odata_getattr(const char* path, struct stat* stats) {
  return OData::FuseAdapter::getInstance().getattr(path, stats);
}

static int odata_access(const char* path, int mask) {
  return OData::FuseAdapter::getInstance().access(path, mask);
}

static int odata_open(const char* path, fuse_file_info* info) {
  return OData::FuseAdapter::getInstance().open(path, info);
}

static int odata_readdir(
    const char* path,
    void* buffer,
    fuse_fill_dir_t filler,
    off_t offset,
    struct fuse_file_info* fi) {
  return OData::FuseAdapter::getInstance().readdir(
      path, buffer, filler, offset, fi);
}

static int odata_read(
    const char* path,
    char* buffer,
    size_t size,
    off_t offset,
    fuse_file_info* info) {
  return OData::FuseAdapter::getInstance().read(
      path, buffer, size, offset, info);
}

static int odata_release(const char* path, fuse_file_info* info) {
  return OData::FuseAdapter::getInstance().release(path, info);
}

struct fuse_operations getOdataOperations() {
  struct fuse_operations ret;
  memset(&ret, 0, sizeof(fuse_operations));
  ret.getattr = odata_getattr;
  ret.access = odata_access;
  ret.open = odata_open;
  ret.read = odata_read;
  ret.release = odata_release;
  ret.readdir = odata_readdir;
  return ret;
}
}

namespace OData {

std::unique_ptr<FuseAdapter> FuseAdapter::singleton;

FuseAdapter& FuseAdapter::createInstance(const Config& config) {
  if (singleton) {
    assert(false);
  }
  singleton.reset(new FuseAdapter(config));
  return *singleton;
}

FuseAdapter& FuseAdapter::getInstance() {
  if (!singleton) {
    assert(false);
  }
  return *singleton;
}

int FuseAdapter::getattr(const char* path, struct stat* attr) {
  const auto file = data_hub.getFile(path);
  if (file == nullptr) {
    return -ENOENT;
  }
  std::memset(attr, 0, sizeof(*attr));
  if (file->isDirectory()) {
    attr->st_mode = S_IFDIR | 0555;
    attr->st_nlink = 2;
  } else {
    attr->st_mode = S_IFREG | 0444;
    attr->st_nlink = 1;
  }
  attr->st_size = file->getSize();
  return 0;
}

int FuseAdapter::access(const char* path, int) {
  if (data_hub.getFile(path) == nullptr) {
    return -ENOENT;
  } else {
    return 0;
  }
}

int FuseAdapter::open(const char* path, fuse_file_info*) {
  try {
    data_hub.getFile(path, 0, 0);
    return 0;
  } catch (...) {
    return -ENOENT;
  }
}

int FuseAdapter::read(
    const char* path,
    char* buffer,
    size_t size,
    off_t offset,
    fuse_file_info*) {
  try {
    const auto content = data_hub.getFile(path, offset, size);
    std::memcpy(buffer, content.data(), content.size());
    return content.size();
  } catch (...) {
    return -ENOENT;
  }
}

int FuseAdapter::release(const char*, fuse_file_info*) {
  return -1;
}

int FuseAdapter::readdir(
    const char* path,
    void* buffer,
    fuse_fill_dir_t filler,
    off_t,
    struct fuse_file_info*) {
  const auto directory = data_hub.getFile(path);
  if (directory == nullptr) {
    return -ENOENT;
  } else if (directory->isDirectory()) {
    filler(buffer, ".", nullptr, 0);
    filler(buffer, "..", nullptr, 0);
    for (const auto& entry : directory->readDir()) {
      if (filler(buffer, entry.c_str(), nullptr, 0) != 0) {
        return -ENOMEM;
      }
    }
    return 0;
  } else {
    return -ENOTDIR;
  }
}

FuseAdapter::FuseAdapter(const Config& config)
    : connection(config.getUrl(), config.getUsername(), config.getPassword()),
      data_hub(
          connection,
          config.getMissions(),
          config.getDbPath(),
          config.getTmpPath(),
          config.getCacheSize()),
      operations(getOdataOperations()) {
}

FuseAdapter::~FuseAdapter() = default;

struct fuse_operations* FuseAdapter::getFuseOperations() {
  return &operations;
}

} /* namespace OData */
