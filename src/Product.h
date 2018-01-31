#ifndef SRC_PRODUCT_H_
#define SRC_PRODUCT_H_

#include "Directory.h"
#include "ProductPath.h"
#include <cstdlib>
#include <iosfwd>
#include <string>
#include <vector>

namespace OData {

class Product {
public:
  Product(
      std::string id,
      std::string name,
      std::string ingestion_date,
      std::string filename,
      std::string platform) noexcept;
  Product(const Product&) = delete;
  Product(Product&&) = default;
  ~Product() = default;
  Product& operator=(const Product&) = delete;
  Product& operator=(Product&&) = default;
  bool operator==(const Product& entry) const;

  void setArchiveStructure(Directory directory) noexcept;
  ProductPath getProductPath() const noexcept;

  void toString(std::ostream& ostr) const noexcept;
  const std::string& getPlatform() const noexcept;
  std::string getManifestFilename() const noexcept;
  const std::string& getFilename() const;

private:
  std::string id;
  std::string name;
  std::string ingestion_date;
  std::string filename;
  std::string platform;
  Directory directory;
};

std::ostream& operator<<(std::ostream& ostr, const Product& product) noexcept;

} /* namespace OData */

#endif /* SRC_PRODUCT_H_ */
