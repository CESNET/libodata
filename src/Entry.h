#ifndef SRC_ENTRY_H_
#define SRC_ENTRY_H_

#include <cstdlib>
#include <string>

namespace OData {

class Entry {
public:
    Entry(
        std::string id,
        std::string name,
        std::string creation_date,
        std::string ingestion_date);
    Entry(const Entry&) = default;
    Entry(Entry&&) = default;
    ~Entry() = default;
    Entry& operator=(const Entry&) = default;
    Entry& operator=(Entry&&) = default;

    std::string id;
    std::string name;
    std::string creation_date;
    std::string ingestion_date;
};

} /* namespace OData */

#endif /* SRC_ENTRY_H_ */
