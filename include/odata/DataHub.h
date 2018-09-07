#ifndef ODATA_DATAHUB_H_
#define ODATA_DATAHUB_H_

#include <boost/filesystem/path.hpp>
#include <cstdint>
#include <memory>
#include <vector>

namespace OData {

class Connection;
class FileSystemNode;
class PathBuilder;
class ProductStorage;

/**
 * Data hub filesystem
 */
class DataHub {
public:
  /**
   * Creates instance of data hub which stores metadata in Berkeley database2
   * @param connection connection to data hub
   * @param missions list of missions downloaded by this instance
   * @param db_path path to database file
   * @param tmp_path path to directory for storing temporary files
   * @param cache_size maximum number of temporary files
   * @param path_builder product path builder
   */
  explicit DataHub(
      Connection& connection,
      const std::vector<std::string>& missions,
      boost::filesystem::path db_path,
      boost::filesystem::path tmp_path,
      std::uint32_t cache_size,
      const PathBuilder& path_builder);

  /**
   * Creates instance of data hub which uses specified data storage
   * @param connection connection to data hub
   * @param missions list of missions downloaded by this instance
   * @param product_storage storage of product metadata
   * @param tmp_path path to directory for storing temporary files
   * @param cache_size maximum number of temporary files
   * @param path_builder product path builder
   * @param timeout_duration_ms timeout in milliseconds for periodic discovery
   * of deleted and added products
   */
  explicit DataHub(
      Connection& connection,
      const std::vector<std::string>& missions,
      std::shared_ptr<ProductStorage> product_storage,
      boost::filesystem::path tmp_path,
      std::uint32_t cache_size,
      const PathBuilder& path_builder,
      std::uint32_t timeout_duration_ms);

  ~DataHub();

  /**
   * @return datahub filesystem root
   */
  std::shared_ptr<FileSystemNode> getData() noexcept;

  /**
   * Get file from data hub filesystem
   * @param file_path path to requested file
   * @return requested file or nullptr if file does not exist
   * @exception OData::DataHubException if error occurs
   */
  std::shared_ptr<FileSystemNode> getFile(
      const boost::filesystem::path& file_path) const;

  /**
   * Get content of file from data hub filesystem
   * @param file_path path to requested file
   * @param offset address of first byte to read
   * @param length how many bytes should be read
   * @return data read from file
   * @exception OData::DataHubException if error occurs or offset and length are
   * out of bounds
   */
  std::vector<char> getFile(
      const boost::filesystem::path& file_path,
      std::size_t offset,
      std::size_t length) const;

private:
  struct Impl;
  std::unique_ptr<Impl> pimpl;
};

} /* namespace OData */

#endif /* ODATA_DATAHUB_H_ */
