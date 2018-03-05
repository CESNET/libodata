#ifndef SRC_PRODUCT_H_
#define SRC_PRODUCT_H_

#include "Directory.h"
#include "FileSystemNode.h"
#include "ProductPath.h"
#include <cstdlib>
#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

namespace OData {

class File;

/**
 * Product stored in Copernicus Open Access Hub
 */
class Product : public FileSystemNode {
public:
  Product(
      std::string id,
      std::string name,
      std::string ingestion_date,
      std::string filename,
      std::string platform,
      std::string type) noexcept;
  Product(const Product&) = delete;
  virtual ~Product() = default;
  Product& operator=(const Product&) = delete;

  void setArchiveStructure(
      std::shared_ptr<Directory> directory,
      std::shared_ptr<File> manifest) noexcept;
  ProductPath getProductPath() const noexcept;

  void toString(std::ostream& ostr, unsigned indent_level = 0) const
      noexcept override;
  bool compare(const FileSystemNode& node) const noexcept override;
  std::string getName() const noexcept override;
  const FileSystemNode* getFile(
      boost::filesystem::path::const_iterator begin,
      boost::filesystem::path::const_iterator end) const noexcept override;
  std::vector<std::string> readDir() const noexcept override;
  bool isDirectory() const noexcept override;
  const std::string& getPlatform() const noexcept;
  std::string getManifestFilename() const noexcept;
  const std::string& getFilename() const noexcept;
  std::string getDate() const noexcept;

private:
  std::string id;
  std::string name;
  std::string ingestion_date;
  std::string filename;
  std::string platform;
  std::string type;
  std::shared_ptr<Directory> directory;
  std::shared_ptr<File> manifest;
};

std::ostream& operator<<(std::ostream& ostr, const Product& product) noexcept;

} /* namespace OData */

#endif /* SRC_PRODUCT_H_ */
