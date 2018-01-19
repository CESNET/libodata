#include "XmlParser.h"

#include <stdexcept>
#include <tinyxml2.h>
#include "Entry.h"

namespace OData {

std::vector<Entry> XmlParser::parseList(
    const std::string& xml) const {
    std::vector<Entry> entries;
    tinyxml2::XMLDocument doc;
    if (tinyxml2::XML_SUCCESS != doc.Parse(xml.c_str())) {
        throw std::invalid_argument("Invalid xml");
    }
    for (auto entry_node = doc.FirstChildElement("feed")->FirstChildElement("entry"); entry_node != nullptr;
            entry_node = entry_node->NextSiblingElement()) {
        const auto properties = entry_node->FirstChildElement("m:properties");
        entries.push_back(Entry(properties->FirstChildElement("d:Id")->GetText(),
                properties->FirstChildElement("d:Name")->GetText(),
                properties->FirstChildElement("d:CreationDate")->GetText(),
                properties->FirstChildElement("d:IngestionDate")->GetText()));
    }
    return entries;
}

} /* namespace OData */
