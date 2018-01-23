#ifndef SRC_PRODUCT_H_
#define SRC_PRODUCT_H_

#include <cstdlib>
#include <string>
#include <vector>

namespace OData {

class Product {
public:
    Product(
        std::string id,
        std::string name,
        std::string ingestion_date,
        std::string filename);
    Product(
        const Product&) = default;
    Product(
        Product&&) = default;
    ~Product() = default;
    Product& operator=(
        const Product&) = default;
    Product& operator=(
        Product&&) = default;
    bool operator==(
        const Product& entry) const;

private:
    std::string id;
    std::string name;
    std::string ingestion_date;
    std::string filename;
    std::vector<std::string> files;
};

} /* namespace OData */

#endif /* SRC_PRODUCT_H_ */
