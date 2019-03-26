#ifndef SRC_CONFIG_H_
#define SRC_CONFIG_H_

#include <memory>
#include <string>
#include <vector>

namespace OData {

class PathBuilder;

/**
 * Program configuration
 */
class Config {
public:
  /**
   * Constructor
   * @param user_home user home directory
   * @param argc program argument count
   * @param argv program argument values
   * @param allow_unknown_arguments if occurence of unknown argument should be
   * considered error
   */
  Config(
      const std::string& user_home,
      int argc,
      char** argv,
      bool allow_unknown_arguments = false) noexcept;

  /**
   * Constructor
   * @param program_name program file name
   * @param user_home user home directory
   * @param config_file path to configuration file
   */
  explicit Config(
      std::string program_name,
      const std::string& user_home,
      const std::string& config_file) noexcept;
  Config(Config&&);
  ~Config();
  Config& operator=(Config&&);

  /**
   * @return program file name
   */
  const std::string& getProgramName() const noexcept;

  /**
   * @return path to database file
   */
  const std::string& getDbPath() const noexcept;

  /**
   * @return list of configured sentinel missions
   */
  const std::vector<std::string>& getMissions() const noexcept;

  /**
   * @return password used for authentication on data hub server
   */
  const std::string& getPassword() const noexcept;

  /**
   * @return path to directory for storing temporary files
   */
  const std::string& getTmpPath() const noexcept;

  /**
   * @return data hub server url
   */
  const std::string& getUrl() const noexcept;

  /**
   * @return user name used for authentication on data hub server
   */
  const std::string& getUsername() const noexcept;

  /**
   * @return if configuration is valid
   */
  bool isValid() const noexcept;

  /**
   * @return if version was requested
   */
  bool printVersion() const noexcept;

  /**
   * @return if help was requested
   */
  bool printHelp() const noexcept;

  /**
   * @return program help
   */
  std::string getHelp() const noexcept;

  /**
   * @return program version
   */
  std::string getVersion() const noexcept;

  /**
   * @return error message if config is not valid
   */
  std::string getErrorMessage() const noexcept;

  /**
   * @return temporary files count
   */
  std::uint32_t getCacheSize() const noexcept;

  /**
   * @return user defined filesystem path builder
   */
  const PathBuilder& getPathBuilder() const noexcept;

  /**
   * @return if data hub certificate should be validated
   */
  bool validateCertificate() const noexcept;

private:
  struct Impl;
  std::unique_ptr<Impl> pimpl;
};

} /* namespace OData */

#endif /* SRC_CONFIG_H_ */
