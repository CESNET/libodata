#ifndef SRC_FUSEADAPTER_H_
#define SRC_FUSEADAPTER_H_

#include "Config.h"
#include "DataHub.h"
#include "DataHubConnection.h"
#include <fuse.h>
#include <memory>

namespace OData {

/**
 * Fuse filesystem C++ adapter.
 */
class FuseAdapter {
public:
  /**
   * @param config program configuration
   * @return singleton instance
   */
  static FuseAdapter& createInstance(Config config);

  /**
   * @return singleton instance
   */
  static FuseAdapter& getInstance();
  FuseAdapter(const FuseAdapter&) = default;
  FuseAdapter(FuseAdapter&&) = default;
  ~FuseAdapter();

  /**
   * @return initialized C fuse structure
   */
  struct fuse_operations* getFuseOperations();

  // fuse operations
  void init();
  void destroy();
  int getattr(const char*, struct stat*);
  int access(const char* path, int mask);
  int open(const char*, fuse_file_info*);
  int readdir(
      const char*, void*, fuse_fill_dir_t, off_t, struct fuse_file_info*);
  int read(const char*, char*, size_t, off_t, fuse_file_info*);
  int release(const char*, fuse_file_info*);

private:
  FuseAdapter(Config config);
  static std::unique_ptr<FuseAdapter> singleton;
  Config config;
  std::unique_ptr<DataHubConnection> connection;
  std::unique_ptr<DataHub> data_hub;
  struct fuse_operations operations;
};

} /* namespace OData */

#endif /* SRC_FUSEADAPTER_H_ */
