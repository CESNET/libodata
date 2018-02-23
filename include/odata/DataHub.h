#ifndef ODATA_DATAHUB_H_
#define ODATA_DATAHUB_H_

#include <memory>

namespace OData {

class Connection;
class FileSystemNode;

class DataHub {
public:
  explicit DataHub(Connection& connection);
  ~DataHub();
  std::unique_ptr<FileSystemNode> getData();

private:
  struct Impl;
  std::unique_ptr<Impl> pimpl;
};

} /* namespace OData */

#endif /* ODATA_DATAHUB_H_ */
