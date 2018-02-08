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

class Product : public FileSystemNode {
public:
  Product(
      std::string id,
      std::string name,
      std::string ingestion_date,
      std::string filename,
      std::string platform) noexcept;
  Product(const Product&) = delete;
  ~Product() = default;
  Product& operator=(const Product&) = delete;

  void setArchiveStructure(std::unique_ptr<Directory> directory) noexcept;
  ProductPath getProductPath() const noexcept;

  void toString(std::ostream& ostr, unsigned indent_level = 0) const
      noexcept override;
  bool compare(const FileSystemNode& node) const noexcept override;
  const std::string& getPlatform() const noexcept;
  std::string getManifestFilename() const noexcept;
  const std::string& getFilename() const;

private:
  std::string id;
  std::string name;
  std::string ingestion_date;
  std::string filename;
  std::string platform;
  std::unique_ptr<Directory> directory;
};

std::ostream& operator<<(std::ostream& ostr, const Product& product) noexcept;

} /* namespace OData */

#endif /* SRC_PRODUCT_H_ */
