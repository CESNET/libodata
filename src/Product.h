#ifndef SRC_PRODUCT_H_
#define SRC_PRODUCT_H_

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
      std::string platform);
  Product(const Product&) = default;
  Product(Product&&) = default;
  ~Product() = default;
  Product& operator=(const Product&) = default;
  Product& operator=(Product&&) = default;
  bool operator==(const Product& entry) const;

  void setFiles(std::vector<std::string> files) noexcept;
  std::string getProductPath() const noexcept;

  void toString(std::ostream& ostr) const;
  const std::string& getPlatform() const;
  std::string getManifestFilename() const;

private:
  std::string id;
  std::string name;
  std::string ingestion_date;
  std::string filename;
  std::string platform;
  std::vector<std::string> files;
};

std::ostream& operator<<(std::ostream& ostr, const Product& product);

} /* namespace OData */

#endif /* SRC_PRODUCT_H_ */
