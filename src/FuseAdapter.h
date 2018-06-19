#ifndef SRC_FUSEADAPTER_H_
#define SRC_FUSEADAPTER_H_

#include "DataHub.h"
#include "DataHubConnection.h"
#include <fuse.h>
#include <memory>

namespace OData {

class Config;

class FuseAdapter {
public:
  static FuseAdapter& createInstance(const Config& config);
  static FuseAdapter& getInstance();
  FuseAdapter(const FuseAdapter&) = default;
  FuseAdapter(FuseAdapter&&) = default;
  ~FuseAdapter();

  struct fuse_operations* getFuseOperations();
  int getattr(const char*, struct stat*);
  int access(const char* path, int mask);
  int open(const char*, fuse_file_info*);
  int readdir(
      const char*, void*, fuse_fill_dir_t, off_t, struct fuse_file_info*);
  int read(const char*, char*, size_t, off_t, fuse_file_info*);
  int release(const char*, fuse_file_info*);

private:
  FuseAdapter(const Config& config);
  static std::unique_ptr<FuseAdapter> singleton;
  DataHubConnection connection;
  DataHub data_hub;
  struct fuse_operations operations;
};

} /* namespace OData */

#endif /* SRC_FUSEADAPTER_H_ */
