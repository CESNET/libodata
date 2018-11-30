#ifndef SRC_PRODUCTFILE_H_
#define SRC_PRODUCTFILE_H_

namespace OData {

class ProductPath;

/**
 * Product file interface.
 */
class ProductFile {
public:
  ProductFile() = default;
  virtual ~ProductFile() = default;
  ProductFile(const ProductFile&) = delete;
  ProductFile& operator=(const ProductFile&) = delete;

  /**
   * @return path to file
   */
  virtual ProductPath getProductPath() const noexcept = 0;
};

} /* namespace OData */

#endif /* SRC_PRODUCTFILE_H_ */
