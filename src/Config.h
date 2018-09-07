#ifndef SRC_CONFIG_H_
#define SRC_CONFIG_H_

#include <memory>
#include <string>
#include <vector>

namespace OData {

class PathBuilder;

class Config {
public:
  Config(
      const std::string& user_home,
      int argc,
      char** argv,
      bool allow_unknown_arguments = false) noexcept;
  explicit Config(
      std::string program_name,
      const std::string& user_home,
      const std::string& config_file) noexcept;
  Config(Config&&);
  ~Config();
  Config& operator=(Config&&);

  const std::string& getProgramName() const noexcept;
  const std::string& getDbPath() const noexcept;
  const std::vector<std::string>& getMissions() const noexcept;
  const std::string& getPassword() const noexcept;
  const std::string& getTmpPath() const noexcept;
  const std::string& getUrl() const noexcept;
  const std::string& getUsername() const noexcept;
  bool isValid() const noexcept;
  bool printVersion() const noexcept;
  bool printHelp() const noexcept;
  std::string getHelp() const noexcept;
  std::string getVersion() const noexcept;
  std::string getErrorMessage() const noexcept;
  std::uint32_t getCacheSize() const noexcept;
  const PathBuilder& getPathBuilder() const noexcept;

private:
  struct Impl;
  std::unique_ptr<Impl> pimpl;
};

} /* namespace OData */

#endif /* SRC_CONFIG_H_ */
