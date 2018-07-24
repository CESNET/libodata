#include "Config.h"
#include "FuseAdapter.h"
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <fuse.h>
#include <iostream>
#include <string>
#include <vector>

namespace OData {

struct Arguments {
  const char* config_file;
  Arguments() : config_file(nullptr) {
  }
};

enum {
  HELP,
  VERSION,
};

#define ODATA_OPT(t, p, v)                                                     \
  { t, offsetof(struct Arguments, p), v }

struct fuse_opt options[] = {ODATA_OPT("odata-cfg=%s", config_file, 0),
                             FUSE_OPT_KEY("-V", VERSION),
                             FUSE_OPT_KEY("--version", VERSION),
                             FUSE_OPT_KEY("-h", HELP),
                             FUSE_OPT_KEY("--help", HELP),
                             FUSE_OPT_END};

void printHelp() {
  std::cout << "usage: %s mountpoint [options]" << std::endl
            << "general options:" << std::endl
            << "    -o opt,[opt...]  mount options" << std::endl
            << "    -h   --help      print help" << std::endl
            << "    -V   --version   print version" << std::endl
            << "odatafs options:" << std::endl
            << "    -o odata-cfg=PATH" << std::endl;
}

int processOptions(void*, const char*, int key, struct fuse_args* outargs) {
  switch (key) {
  case HELP: {
    printHelp();
    fuse_opt_add_arg(outargs, "-ho");
    fuse_operations dummy;
    memset(&dummy, 0, sizeof(fuse_operations));
    fuse_main(outargs->argc, outargs->argv, &dummy, nullptr);
    std::exit(1);
  }
  case VERSION: {
    std::cout << "odatfs version "
              << "0.0" << std::endl;
    fuse_opt_add_arg(outargs, "--version");
    fuse_operations dummy;
    memset(&dummy, 0, sizeof(fuse_operations));
    fuse_main(outargs->argc, outargs->argv, &dummy, nullptr);
    std::exit(0);
  }
  }
  return 1;
}
} // namespace OData

int main(int argc, char* argv[]) {
  struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
  OData::Arguments arguments;
  if (fuse_opt_parse(&args, &arguments, OData::options, OData::processOptions)
      == -1) {
    std::cerr << argv[0] << " invalid arguments" << std::endl;
    OData::printHelp();
    return 1;
  }
  if (arguments.config_file == nullptr) {
    std::cerr << argv[0] << " configuration file not provided" << std::endl;
    OData::printHelp();
    return 1;
  }
  OData::Config config(std::getenv("HOME"), arguments.config_file);
  if (!config.isValid()) {
    std::cerr << "Invalid configuration file '" << arguments.config_file
              << "': " << config.getErrorMessage() << std::endl;
    std::cout << config.getHelp() << std::endl;
    return 1;
  }
  OData::FuseAdapter& fuse = OData::FuseAdapter::createInstance(config);
  return fuse_main(args.argc, args.argv, fuse.getFuseOperations(), nullptr);
}
