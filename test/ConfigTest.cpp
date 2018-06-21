#include "Config.h"
#include <gtest/gtest.h>

namespace OData {
namespace Test {

TEST(ConfigTest, FileConfigTest) {
  Config instance("examples/test.cfg");
  ASSERT_TRUE(instance.isValid());
  ASSERT_EQ("http://test.url:8089", instance.getUrl());
  ASSERT_EQ("testname", instance.getUsername());
  ASSERT_EQ("testpassword", instance.getPassword());
  std::vector<std::string> test_missions{"mission1", "mission2", "mission3"};
  ASSERT_EQ(test_missions, instance.getMissions());
  ASSERT_EQ("/path/to/db/directory", instance.getDbPath());
  ASSERT_EQ("/path/to/tmp/directory", instance.getTmpPath());
}

TEST(ConfigTest, CommandLineArgumentsTest) {
  {
    const char* arguments[] = {"test-program-name",
                               "--url",
                               "http://test.url:8089",
                               "--username",
                               "testname",
                               "--password",
                               "testpassword",
                               "--db_path",
                               "/path/to/db/directory",
                               "--tmp_path",
                               "/path/to/tmp/directory",
                               "--missions",
                               "mission1,mission2,mission3"};
    Config instance(
        sizeof(arguments) / sizeof(*arguments), const_cast<char**>(arguments));
    ASSERT_TRUE(instance.isValid());
    ASSERT_EQ("http://test.url:8089", instance.getUrl());
    ASSERT_EQ("testname", instance.getUsername());
    ASSERT_EQ("testpassword", instance.getPassword());
    std::vector<std::string> test_missions{"mission1", "mission2", "mission3"};
    ASSERT_EQ(test_missions, instance.getMissions());
    ASSERT_EQ("/path/to/db/directory", instance.getDbPath());
    ASSERT_EQ("/path/to/tmp/directory", instance.getTmpPath());
    ASSERT_TRUE(!instance.printHelp());
    ASSERT_TRUE(!instance.printVersion());
  }
  {
    const char* arguments[] = {"test-program-name", "--help"};
    Config instance(
        sizeof(arguments) / sizeof(*arguments), const_cast<char**>(arguments));
    ASSERT_TRUE(instance.isValid());
    ASSERT_TRUE(instance.printHelp());
    ASSERT_TRUE(!instance.printVersion());
  }
  {
    const char* arguments[] = {"test-program-name", "--version"};
    Config instance(
        sizeof(arguments) / sizeof(*arguments), const_cast<char**>(arguments));
    ASSERT_TRUE(instance.isValid());
    ASSERT_TRUE(!instance.printHelp());
    ASSERT_TRUE(instance.printVersion());
  }
}

TEST(ConfigTest, InvalidArgumentsTest) {
  {
    const char* unknown[] = {"test-program-name", "--c", "invalid"};
    Config instance(
        sizeof(unknown) / sizeof(*unknown), const_cast<char**>(unknown));
    ASSERT_TRUE(!instance.isValid());
    ASSERT_TRUE(!instance.printHelp());
    ASSERT_TRUE(!instance.printVersion());
  }
  {
    const char* missing[] = {"test-program-name",
                             "--url",
                             "http://test.url:8089",
                             "--username",
                             "testname",
                             "--password",
                             "testpassword"};
    Config instance(
        sizeof(missing) / sizeof(*missing), const_cast<char**>(missing));
    ASSERT_TRUE(!instance.isValid());
    ASSERT_TRUE(!instance.printHelp());
    ASSERT_TRUE(!instance.printVersion());
  }
}

} // namespace Test
} // namespace OData
