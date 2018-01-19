#include "Entry.h"

namespace OData {
Entry::Entry(
    std::string id,
    std::string name,
    std::string creation_date,
    std::string ingestion_date) :
        id(std::move(id)),
        name(std::move(name)),
        creation_date(std::move(creation_date)),
        ingestion_date(std::move(ingestion_date)) {
}

} /* namespace OData */
