#ifndef ODATA_DATAHUB_H_
#define ODATA_DATAHUB_H_

#include <boost/filesystem/path.hpp>
#include <cstdint>
#include <memory>
#include <vector>

namespace OData {

class Connection;
class FileSystemNode;
class ProductStorage;

class DataHub {
public:
  DataHub(
      Connection& connection,
      const std::vector<std::string>& missions,
      boost::filesystem::path db_path,
      boost::filesystem::path tmp_path,
      std::uint32_t cache_size);

  explicit DataHub(
      Connection& connection,
      const std::vector<std::string>& missions,
      std::shared_ptr<ProductStorage> product_storage,
      boost::filesystem::path tmp_path,
      std::uint32_t cache_size);
  ~DataHub();
  std::shared_ptr<FileSystemNode> getData();
  std::shared_ptr<FileSystemNode> getFile(
      const boost::filesystem::path& file_path);
  std::vector<char> getFile(
      const boost::filesystem::path& file_path,
      std::size_t offset,
      std::size_t length);

private:
  struct Impl;
  std::unique_ptr<Impl> pimpl;
};

} /* namespace OData */

#endif /* ODATA_DATAHUB_H_ */
