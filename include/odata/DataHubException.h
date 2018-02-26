#ifndef ODATA_DATAHUBEXCEPTION_H_
#define ODATA_DATAHUBEXCEPTION_H_

#include <stdexcept>
#include <string>

namespace OData {

class DataHubException : public std::exception {
public:
  DataHubException(std::string msg) noexcept;
  DataHubException(
      const std::string& operation, const std::string& error) noexcept;
  virtual ~DataHubException() = default;
  const char* what() const noexcept;

private:
  std::string msg;
};

} /* namespace OData */

#endif /* ODATA_DATAHUBEXCEPTION_H_ */
