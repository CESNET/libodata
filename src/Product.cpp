#include "Product.h"

namespace OData {

Product::Product(
    std::string id,
    std::string name,
    std::string ingestion_date,
    std::string filename) :
        id(std::move(id)),
        name(std::move(name)),
        ingestion_date(std::move(ingestion_date)),
        filename(std::move(filename)),
        files() {
}

bool Product::operator ==(
    const Product& entry) const {
    return id == entry.id && name == entry.name && ingestion_date == entry.ingestion_date && filename == entry.filename
            && files == entry.files;
}

} /* namespace OData */
