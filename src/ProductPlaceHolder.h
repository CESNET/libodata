#ifndef SRC_PRODUCTPLACEHOLDER_H_
#define SRC_PRODUCTPLACEHOLDER_H_

#include "FileSystemNode.h"
#include <memory>
#include <string>

namespace OData {

class Product;
class ProductStorage;

class ProductPlaceHolder : public FileSystemNode {
public:
  ProductPlaceHolder(
      std::string id,
      std::string name,
      std::shared_ptr<ProductStorage> product_storage);
  virtual ~ProductPlaceHolder() = default;

  void toString(std::ostream& ostr, unsigned indent_level = 0) const override;
  bool compare(const FileSystemNode& node) const noexcept override;
  std::string getName() const noexcept override;
  void setName(std::string name) noexcept override;
  std::shared_ptr<FileSystemNode> getFile(
      boost::filesystem::path::const_iterator begin,
      boost::filesystem::path::const_iterator end) const noexcept override;
  std::vector<std::string> readDir() const noexcept override;
  bool isDirectory() const noexcept override;
  std::size_t getSize() const noexcept override;
  std::shared_ptr<FileSystemNode> getChild(const std::string& name) const
      noexcept override;
  void removeChild(const std::string& child_name) noexcept override;
  void addChild(std::shared_ptr<FileSystemNode> child) noexcept override;

private:
  std::shared_ptr<Product> getInstance() const noexcept;

  std::string id;
  std::string name;
  std::shared_ptr<ProductStorage> product_storage;
};

} /* namespace OData */

#endif /* SRC_PRODUCTPLACEHOLDER_H_ */
