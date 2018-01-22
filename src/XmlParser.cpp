#include "XmlParser.h"

#include <stdexcept>
#include <tinyxml2.h>
#include "Entry.h"

namespace OData {
namespace {

class XmlDocument {
public:
    XmlDocument(
        const std::string& xml);
    ~XmlDocument() = default;
    std::vector<const tinyxml2::XMLElement*> filter(
        const tinyxml2::XMLElement* root,
        std::string node_name) const noexcept;
    const tinyxml2::XMLElement* filterOne(
        const tinyxml2::XMLElement* root,
        std::string node_name) const;
    std::string getPropertyValue(
        const tinyxml2::XMLElement* entry_node,
        const std::string& property_name) const;

    tinyxml2::XMLDocument doc;
    std::vector<const tinyxml2::XMLElement*> entries;
};

class XmlFilter: public tinyxml2::XMLVisitor {
public:
    explicit XmlFilter(
        std::string node_name) noexcept;
    virtual ~XmlFilter() = default;
    bool VisitEnter(
        const tinyxml2::XMLElement& element,
        const tinyxml2::XMLAttribute*) override;
    const std::string node_name;
    std::vector<const tinyxml2::XMLElement*> filtered;
};

XmlDocument::XmlDocument(
    const std::string& xml) :
        doc(),
        entries() {
    if (tinyxml2::XML_SUCCESS != doc.Parse(xml.c_str())) {
        throw std::invalid_argument("Invalid xml");
    }
    entries = std::move(filter(doc.RootElement(), "entry"));
}

std::vector<const tinyxml2::XMLElement*> XmlDocument::filter(
    const tinyxml2::XMLElement* root,
    std::string node_name) const noexcept {
    XmlFilter filter(std::move(node_name));
    root->Accept(&filter);
    return std::move(filter.filtered);
}

const tinyxml2::XMLElement* XmlDocument::filterOne(
    const tinyxml2::XMLElement* root,
    std::string node_name) const {
    const auto nodes = filter(root, std::move(node_name));
    if (nodes.size() == 1) {
        return nodes[0];
    } else {
        throw std::invalid_argument("Invalid xml");
    }
}

std::string XmlDocument::getPropertyValue(
    const tinyxml2::XMLElement* entry_node,
    const std::string& property_name) const {
    return filterOne(entry_node, "d:" + property_name)->GetText();
}

XmlFilter::XmlFilter(
    std::string node_name) noexcept :
        node_name(std::move(node_name)),
        filtered() {

}

bool XmlFilter::VisitEnter(
    const tinyxml2::XMLElement& element,
    const tinyxml2::XMLAttribute*) {
    if (element.Name() == node_name) {
        filtered.push_back(&element);
        return false;
    } else {
        return true;
    }
}

} /* namespace */

std::vector<Entry> XmlParser::parseList(
    const std::string& xml) const {
    XmlDocument doc(xml);
    std::vector<Entry> entries;
    for (const auto& entry_node : doc.entries) {
        entries.push_back(Entry(doc.getPropertyValue(entry_node, "Id"),
                doc.getPropertyValue(entry_node, "Name"),
                doc.getPropertyValue(entry_node, "CreationDate"),
                doc.getPropertyValue(entry_node, "IngestionDate")));
    }
    return entries;
}

std::string XmlParser::parseFilename(
    const std::string& xml) const {
    XmlDocument doc(xml);
    for (const auto& entry_node : doc.entries) {
        if (doc.getPropertyValue(entry_node, "Id") == "Filename") {
            return doc.getPropertyValue(entry_node, "Value");
        }
    }
    throw std::invalid_argument("Invalid xml");
}
} /* namespace OData */
