#include "XmlParser.h"

#include "DataHubException.h"
#include "Product.h"
#include <algorithm>
#include <functional>
#include <tinyxml2.h>

namespace OData {
namespace {

class XmlFilter : public tinyxml2::XMLVisitor {
public:
  explicit XmlFilter(
      std::function<bool(const tinyxml2::XMLElement&)> filter) noexcept
      : filtered(), filter(filter) {
  }

  virtual ~XmlFilter() = default;

  bool VisitEnter(
      const tinyxml2::XMLElement& element,
      const tinyxml2::XMLAttribute*) override {
    if (filter(element)) {
      filtered.push_back(&element);
      return false;
    } else {
      return true;
    }
  }

  std::vector<const tinyxml2::XMLElement*> filtered;
  std::function<bool(const tinyxml2::XMLElement&)> filter;
};

class XmlDocument {
public:
  explicit XmlDocument(const std::vector<char>& xml) : doc() {
    tinyxml2::XMLError xml_status = doc.Parse(xml.data(), xml.size());
    if (tinyxml2::XML_SUCCESS != xml_status) {
      throw DataHubException(
          "XML parsing error", doc.ErrorIDToName(xml_status));
    }
  }

  ~XmlDocument() = default;

  std::vector<const tinyxml2::XMLElement*> filter(
      const tinyxml2::XMLElement* root,
      std::function<bool(const tinyxml2::XMLElement&)> predicate) const {
    XmlFilter filter(predicate);
    root->Accept(&filter);
    return std::move(filter.filtered);
  }

  template <typename T>
  std::vector<T> filterMap(
      const std::string& node_name,
      std::function<T(const tinyxml2::XMLElement*)> map) const {
    std::vector<T> result;
    for (const auto& node :
         filter(doc.RootElement(), [&](const tinyxml2::XMLElement& element) {
           return element.Name() == node_name;
         })) {
      result.push_back(map(node));
    }
    return result;
  }

  std::vector<const tinyxml2::XMLElement*> getEntries() const {
    return filter(doc.RootElement(), [](const tinyxml2::XMLElement& element) {
      return 0 == std::strcmp(element.Name(), "entry");
    });
  }

  std::string getPropertyValue(
      const tinyxml2::XMLElement* entry_node,
      const std::string& property_name) const {
    auto predicate = [&](const tinyxml2::XMLElement& element) {
      return element.Attribute("name", property_name.c_str());
    };
    const auto nodes = filter(entry_node, predicate);
    if (nodes.size() == 1) {
      return nodes[0]->GetText();
    } else if (nodes.empty()) {
      throw DataHubException(property_name, "XML element not found");
    } else {
      throw DataHubException(property_name, "Only one XML element expected");
    }
  }

  tinyxml2::XMLDocument doc;
};

} /* namespace */

std::vector<std::shared_ptr<Product>> XmlParser::parseList(
    const std::vector<char>& xml) const {
  XmlDocument doc(xml);
  std::vector<std::shared_ptr<Product>> products;
  for (const auto& entry_node : doc.getEntries()) {
    products.push_back(std::make_shared<Product>(
        doc.getPropertyValue(entry_node, "uuid"),
        doc.getPropertyValue(entry_node, "identifier"),
        doc.getPropertyValue(entry_node, "ingestiondate"),
        doc.getPropertyValue(entry_node, "filename"),
        doc.getPropertyValue(entry_node, "platformname"),
        doc.getPropertyValue(entry_node, "producttype")));
  }
  return products;
}

std::vector<boost::filesystem::path> XmlParser::parseManifest(
    const std::vector<char>& manifest) const {
  XmlDocument doc(manifest);
  std::function<boost::filesystem::path(const tinyxml2::XMLElement*)> map =
      [](const tinyxml2::XMLElement* node) {
        return node->Attribute("href") + 2;
      };
  return doc.filterMap("fileLocation", map);
}

} /* namespace OData */
