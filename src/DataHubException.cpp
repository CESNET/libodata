#include "DataHubException.h"

namespace OData {

DataHubException::DataHubException(std::string msg) noexcept
    : msg(std::move(msg)) {
}

DataHubException::DataHubException(
    const std::string& operation, const std::string& error) noexcept
    : msg(operation + ": " + error) {
}

const char* DataHubException::what() const noexcept {
  return msg.c_str();
}

} /* namespace OData */
