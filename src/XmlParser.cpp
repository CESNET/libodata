#include "XmlParser.h"

#include "DataHubException.h"
#include "Product.h"
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <cstdlib>
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
      throw DataHubException("XML parsing error", doc.ErrorName());
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

  const tinyxml2::XMLElement* filterOne(
      const tinyxml2::XMLElement* root,
      std::function<bool(const tinyxml2::XMLElement&)> predicate) const {
    const auto nodes = filter(root, std::move(predicate));
    if (nodes.size() == 1) {
      return nodes[0];
    } else if (nodes.empty()) {
      throw DataHubException("XML element not found");
    } else {
      throw DataHubException("Only one XML element expected");
    }
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
    return filterOne(
               entry_node,
               [&](const tinyxml2::XMLElement& element) {
                 return element.Attribute("name", property_name.c_str());
               })
        ->GetText();
  }

  const tinyxml2::XMLElement* getChild(
      const tinyxml2::XMLElement* node, const std::string& name) const {
    return filterOne(node, [&](const tinyxml2::XMLElement& element) {
      // remove namespace
      const auto name_with_namespace = element.Name();
      std::vector<std::string> splited;
      boost::split(
          splited, name_with_namespace, [](char c) { return c == ':'; });
      std::string name_without_namespace;
      if (splited.size() == 2) {
        name_without_namespace = splited[1];
      } else {
        name_without_namespace = splited[0];
      }
      return name_without_namespace == name;
    });
  }

  std::map<std::string, std::string> getProductProperties(
      const tinyxml2::XMLElement* node) const {
    std::map<std::string, std::string> properties;
    for (const auto& node :
         filter(node, [&](const tinyxml2::XMLElement& element) {
           return element.Attribute("name") != nullptr;
         })) {
      properties[node->Attribute("name")] = node->GetText();
    }
    return properties;
  }

  tinyxml2::XMLDocument doc;
};

} /* namespace */

std::vector<std::shared_ptr<Product>> XmlParser::parseList(
    const std::vector<char>& xml) const {
  XmlDocument doc(xml);
  std::vector<std::shared_ptr<Product>> products;
  for (const auto& entry_node : doc.getEntries()) {
    products.push_back(
        std::make_shared<Product>(doc.getProductProperties(entry_node)));
  }
  return products;
}

std::vector<std::pair<boost::filesystem::path, std::size_t>> XmlParser::
    parseManifest(const std::vector<char>& manifest) const {
  XmlDocument doc(manifest);
  std::function<std::pair<boost::filesystem::path, std::size_t>(
      const tinyxml2::XMLElement*)>
      map = [&](const tinyxml2::XMLElement* node) {
        const auto* stream = doc.getChild(node, "byteStream");
        const auto* location = doc.getChild(node, "fileLocation");
        return std::make_pair(
            boost::filesystem::path(location->Attribute("href") + 2),
            static_cast<std::size_t>(std::atoll(stream->Attribute("size"))));
      };
  return doc.filterMap("dataObject", map);
}

std::vector<std::string> OData::XmlParser::parseDeletedList(
    const std::vector<char>& xml) const {
  XmlDocument doc(xml);
  std::function<std::string(const tinyxml2::XMLElement*)> map =
      [&](const tinyxml2::XMLElement* node) {
        return doc.getChild(node, "Id")->GetText();
      };
  return doc.filterMap("entry", map);
}

} /* namespace OData */
