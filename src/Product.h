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

  void setArchiveStructure(std::unique_ptr<Directory> directory) noexcept;
  ProductPath getProductPath() const noexcept;

  void toString(std::ostream& ostr, unsigned indent_level = 0) const
      noexcept override;
  bool compare(const FileSystemNode& node) const noexcept override;
  std::string getName() const noexcept override;
  FileSystemNode* getFile(std::list<std::string> path) const noexcept override;
  std::vector<std::string> readDir() const noexcept override;
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
  std::unique_ptr<Directory> directory;
};

std::ostream& operator<<(std::ostream& ostr, const Product& product) noexcept;

} /* namespace OData */

#endif /* SRC_PRODUCT_H_ */
