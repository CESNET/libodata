#include "PathBuilder.h"

#include "DataHubException.h"
#include "Product.h"
#include <boost/algorithm/string.hpp>
#include <functional>
#include <string>
#include <tuple>

namespace OData {
namespace {
class PathNode {
public:
  PathNode() = default;
  virtual ~PathNode() = default;
  PathNode(const PathNode&) = delete;
  PathNode& operator=(const PathNode&) = delete;
  virtual std::string getValue(const Product& product) const noexcept = 0;
};

class Value : public PathNode {
public:
  explicit Value(std::string value) : value(std::move(value)) {
  }

  virtual ~Value() = default;

  std::string getValue(const Product&) const noexcept override {
    return value;
  }

private:
  std::string value;
};

class Keyword : public PathNode {
public:
  explicit Keyword(std::string keyword) : keyword(std::move(keyword)) {
  }

  virtual ~Keyword() = default;

  std::string getValue(const Product& product) const noexcept override {
    auto value = product.getAttribute(keyword);
    if (value) {
      return value.get();
    } else {
      return "notfound";
    };
  }

private:
  std::string keyword;
};

class Concatenate : public PathNode {
public:
  Concatenate(std::unique_ptr<PathNode> left, std::unique_ptr<PathNode> right)
      : left(std::move(left)), right(std::move(right)) {
  }

  virtual ~Concatenate() = default;

  std::string getValue(const Product& product) const noexcept override {
    return left->getValue(product) + right->getValue(product);
  }

private:
  std::unique_ptr<PathNode> left;
  std::unique_ptr<PathNode> right;
};
} // namespace

struct PathBuilder::Impl {
  Impl(const std::string& path_template) : path() {
    std::vector<std::string> splited;
    boost::split(splited, path_template, [](char c) { return c == '/'; });
    for (const auto& node : splited) {
      if (!node.empty()) {
        splitNode(node);
      }
    }
  }

  ~Impl() = default;

  void splitNode(const std::string& node) {
    auto in_keyword = false;
    auto token_start = 0u;
    std::unique_ptr<PathNode> path_node;
    for (auto i = 0u; i < node.size(); ++i) {
      switch (node[i]) {
      case '$': {
        if (i != token_start) {
          path_node = simplyfiNode(
              std::move(path_node),
              std::make_unique<Value>(
                  std::string(node.substr(token_start, i - token_start))));
        }

        if (in_keyword) {
          throw DataHubException("Unexpected '$' in keyword: " + node);
        }
        if (i + 1 >= node.size() || node[i + 1] != '{') {
          throw DataHubException("'{' expected after '$': " + node);
        }
        in_keyword = true;
        token_start = i + 2;
        ++i;
        break;
      }
      case '{':
        throw DataHubException("Unexpected character '{': " + node);
      case '}': {
        if (in_keyword) {
          in_keyword = false;
          path_node = simplyfiNode(
              std::move(path_node),
              std::make_unique<Keyword>(
                  std::string(node.substr(token_start, i - token_start))));
          token_start = i + 1;
        } else {
          throw DataHubException("Unexpected character '}': " + node);
        }
        break;
      }
      }
    }
    if (in_keyword) {
      throw DataHubException("Expected character '}': " + node);
    }
    if (token_start < node.size()) {
      path_node = simplyfiNode(
          std::move(path_node),
          std::make_unique<Value>(std::string(
              node.substr(token_start, node.size() - token_start))));
    }
    path.emplace_back(std::move(path_node));
  }

  std::unique_ptr<PathNode> simplyfiNode(
      std::unique_ptr<PathNode> left, std::unique_ptr<PathNode> right) const {
    if (left == nullptr) {
      return right;
    } else {
      return std::make_unique<Concatenate>(std::move(left), std::move(right));
    }
  }

  std::vector<std::unique_ptr<PathNode>> path;
};

PathBuilder::PathBuilder() : PathBuilder("") {
}

PathBuilder::PathBuilder(const std::string& path_template)
    : pimpl(std::make_unique<Impl>(path_template)) {
}

PathBuilder::PathBuilder(PathBuilder&&) = default;

PathBuilder::~PathBuilder() = default;

PathBuilder& PathBuilder::operator=(PathBuilder&&) = default;

boost::filesystem::path PathBuilder::createPath(const Product& product) const
    noexcept {
  if (pimpl->path.empty()) {
    return boost::filesystem::path();
  }
  boost::filesystem::path path("/");
  for (const auto& node : pimpl->path) {
    path.append(node->getValue(product));
  }
  return path;
}

} /* namespace OData */
