#include "Config.h"

#include <boost/algorithm/string.hpp>
#include <boost/any.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/value_semantic.hpp>
#include <boost/program_options/variables_map.hpp>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <sstream>

namespace OData {
namespace {
struct Missions {
  Missions() = default;
  Missions(std::vector<std::string> missions) : missions(std::move(missions)) {
  }

  std::vector<std::string> missions;
};

void validate(
    boost::any& v, const std::vector<std::string>& values, Missions*, int) {
  boost::program_options::validators::check_first_occurrence(v);
  const auto s = boost::program_options::validators::get_single_string(values);
  std::vector<std::string> splited;
  boost::split(splited, s, [](char c) { return c == ','; });
  v = boost::any(Missions(std::move(splited)));
}
} // namespace

struct Config::Impl {
  std::string url;
  std::string username;
  std::string password;
  std::vector<std::string> missions;
  std::string db_path;
  std::string tmp_path;
  std::string error;
  bool print_version;
  bool print_help;
  boost::program_options::options_description options;

  Impl(boost::program_options::options_description options)
      : url(),
        username(),
        password(),
        missions(),
        db_path(),
        tmp_path(),
        error(),
        print_version(false),
        print_help(false),
        options(std::move(options)) {
  }

  Impl(int argc, char** argv, bool allow_unknown_arguments)
      : Impl(createCommandLineOptions(
            std::string("Usage: ") + argv[0] + " [options]")) {
    try {
      if (allow_unknown_arguments) {
        initialize(boost::program_options::command_line_parser(argc, argv)
                       .options(options)
                       .allow_unregistered()
                       .run());
      } else {
        initialize(
            boost::program_options::parse_command_line(argc, argv, options));
      }
    } catch (std::exception& ex) {
      error = ex.what();
    }
  }

  Impl(const std::string& config_file) : Impl(createOptions("")) {
    try {
      initialize(boost::program_options::parse_config_file<char>(
          config_file.c_str(), options, false));
    } catch (std::exception& ex) {
      error = ex.what();
    }
  }

  boost::program_options::options_description createOptions(
      const std::string& caption) {
    const boost::filesystem::path home = std::getenv("HOME");
    boost::program_options::options_description options(caption);
    options.add_options()(
        "url", boost::program_options::value(&url), "data hub url")(
        "username",
        boost::program_options::value(&username),
        "data hub user name")(
        "password",
        boost::program_options::value(&password),
        "data hub password")(
        "missions",
        boost::program_options::value<Missions>(),
        "list of mission names as comma separated values")(
        "db_path",
        boost::program_options::value(&db_path)->default_value(
            (home / ".db" / "products.db").string()),
        "database path on local filesystem")(
        "tmp_path",
        boost::program_options::value(&tmp_path)->default_value(
            (home / ".cache" / "odata").string()),
        "temporary files path on local filesystem");
    return options;
  }

  boost::program_options::options_description createCommandLineOptions(
      const std::string& caption) {
    auto options = createOptions(caption);
    options.add_options()("help,h", "print help")(
        "version,v", "print program version");
    return options;
  }

  void initialize(
      const boost::program_options::basic_parsed_options<char>& options) {
    boost::program_options::variables_map values;
    boost::program_options::store(options, values);
    boost::program_options::notify(values);
    if (values.count("missions")) {
      missions = std::move(values["missions"].as<Missions>().missions);
    }
    print_help = values.count("help");
    print_version = values.count("version");
  }
};
Config::Config(int argc, char** argv, bool allow_unknown_arguments) noexcept
    : pimpl(new Impl(argc, argv, allow_unknown_arguments)) {
}

Config::Config(const std::string& config_file) noexcept
    : pimpl(new Impl(config_file)) {
}

Config::~Config() = default;

const std::string& Config::getDbPath() const noexcept {
  return pimpl->db_path;
}

const std::vector<std::string>& Config::getMissions() const noexcept {
  return pimpl->missions;
}

const std::string& Config::getPassword() const noexcept {
  return pimpl->password;
}

const std::string& Config::getTmpPath() const noexcept {
  return pimpl->tmp_path;
}

const std::string& Config::getUrl() const noexcept {
  return pimpl->url;
}

const std::string& Config::getUsername() const noexcept {
  return pimpl->username;
}

bool Config::isValid() const noexcept {
  return pimpl->print_help || pimpl->print_version
         || (pimpl->error.empty() && !pimpl->url.empty()
             && !pimpl->username.empty() && !pimpl->password.empty()
             && !pimpl->missions.empty());
}

bool Config::printVersion() const noexcept {
  return pimpl->print_version;
}

bool Config::printHelp() const noexcept {
  return pimpl->print_help;
}

std::string Config::getHelp() const noexcept {
  std::stringstream stream;
  pimpl->options.print(stream);
  return stream.str();
}

std::string Config::getVersion() const noexcept {
  return "Not implemented yet";
}

std::string Config::getErrorMessage() const noexcept {
  if (!pimpl->error.empty()) {
    return pimpl->error;
  } else {
    std::stringstream stream;
    stream << "Missing arguments: ";
    if (pimpl->url.empty()) {
      stream << "url ";
    }
    if (pimpl->username.empty()) {
      stream << "username ";
    }
    if (pimpl->password.empty()) {
      stream << "password ";
    }
    if (pimpl->missions.empty()) {
      stream << "missions ";
    }
    return stream.str();
  }
}

} /* namespace OData */
