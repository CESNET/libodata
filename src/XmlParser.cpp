#include "XmlParser.h"

#include "Product.h"
#include <functional>
#include <stdexcept>
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
  XmlDocument(const std::string& xml) : doc() {
    if (tinyxml2::XML_SUCCESS != doc.Parse(xml.c_str())) {
      throw std::invalid_argument("Invalid xml");
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
    for (const auto& node : filter(doc.RootElement(), [&](const auto& element) {
           return element.Name() == node_name;
         })) {
      result.push_back(map(node));
    }
    return result;
  }

  const tinyxml2::XMLElement* filterOne(
      const tinyxml2::XMLElement* root,
      std::function<bool(const tinyxml2::XMLElement&)> predicate) const {
    const auto nodes = filter(root, predicate);
    if (nodes.size() == 1) {
      return nodes[0];
    } else {
      throw std::invalid_argument("Invalid xml");
    }
  }

  std::vector<const tinyxml2::XMLElement*> getEntries() const {
    return filter(doc.RootElement(), [](const auto& element) {
      return 0 == std::strcmp(element.Name(), "entry");
    });
  }

  std::string getPropertyValue(
      const tinyxml2::XMLElement* entry_node,
      const std::string& property_name) const {
    auto predicate = [&](const tinyxml2::XMLElement& element) {
      return element.Attribute("name", property_name.c_str());
    };
    return filterOne(entry_node, predicate)->GetText();
  }

  tinyxml2::XMLDocument doc;
};

} /* namespace */

std::vector<std::unique_ptr<Product>> XmlParser::parseList(
    const std::string& xml) const {
  XmlDocument doc(xml);
  std::vector<std::unique_ptr<Product>> products;
  for (const auto& entry_node : doc.getEntries()) {
    products.push_back(std::make_unique<Product>(
        doc.getPropertyValue(entry_node, "uuid"),
        doc.getPropertyValue(entry_node, "identifier"),
        doc.getPropertyValue(entry_node, "ingestiondate"),
        doc.getPropertyValue(entry_node, "filename"),
        doc.getPropertyValue(entry_node, "platformname")));
  }
  return products;
}

std::vector<std::string> XmlParser::parseManifest(
    const std::string& manifest) const {
  XmlDocument doc(manifest);
  std::function<std::string(const tinyxml2::XMLElement*)> map = [](auto node) {
    return node->Attribute("href");
  };
  return doc.filterMap("fileLocation", map);
}

} /* namespace OData */
