#ifndef ODATA_DATAHUB_H_
#define ODATA_DATAHUB_H_

#include <boost/filesystem/path.hpp>
#include <memory>
#include <vector>

namespace OData {

class Connection;
class FileSystemNode;

class DataHub {
public:
  explicit DataHub(
      Connection& connection, const std::vector<std::string>& missions);
  ~DataHub();
  std::shared_ptr<FileSystemNode> getData();
  std::vector<char> getFile(const boost::filesystem::path& file_path);

private:
  struct Impl;
  std::unique_ptr<Impl> pimpl;
};

} /* namespace OData */

#endif /* ODATA_DATAHUB_H_ */
