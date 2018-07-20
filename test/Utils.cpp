#include "Utils.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace OData {
namespace Test {
std::vector<char> readTestInstance(const std::string& filename) {
  std::fstream file("examples/" + filename, std::fstream::in);
  if (file.is_open()) {
    std::stringstream buffer;
    buffer << file.rdbuf();
    const auto data = buffer.str();
    return std::vector<char>(data.begin(), data.end());
  } else {
    throw std::invalid_argument("Cannot open file " + filename);
  }
}
} /* namespace Test */
} /* namespace OData */
