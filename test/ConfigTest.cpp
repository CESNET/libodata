#include "Config.h"

#include "PathBuilder.h"
#include "Product.h"
#include "Utils.h"
#include <boost/filesystem.hpp>
#include <gtest/gtest.h>

namespace OData {
namespace Test {

namespace {
struct ConfigTest : public ::testing::Test {
  void SetUp() override {
  }

  void TearDown() override {
    boost::filesystem::remove_all("path");
    boost::filesystem::remove_all("test_home");
  }
};
} // namespace

TEST_F(ConfigTest, FileConfigTest) {
  Config instance("unitest", "test_home", "examples/test.cfg");
  ASSERT_TRUE(instance.isValid());
  ASSERT_EQ("http://test.url:8089", instance.getUrl());
  ASSERT_EQ("testname", instance.getUsername());
  ASSERT_EQ("testpassword", instance.getPassword());
  std::vector<std::string> test_missions{"mission1", "mission2", "mission3"};
  ASSERT_EQ(test_missions, instance.getMissions());
  ASSERT_EQ("path/to/db/directory/file.db", instance.getDbPath());
  ASSERT_EQ("path/to/tmp/directory", instance.getTmpPath());
  ASSERT_EQ(20, instance.getCacheSize());
  ASSERT_TRUE(boost::filesystem::exists("path/to/db/directory"));
  ASSERT_TRUE(boost::filesystem::exists("path/to/tmp/directory"));
  ASSERT_EQ(
      boost::filesystem::path("/type/2018/11/16/id").string(),
      instance.getPathBuilder()
          .createPath(*createProduct("id", "platform"))
          .string());
}

TEST_F(ConfigTest, CommandLineArgumentsTest) {
  {
    const char* arguments[] = {"test-program-name",
                               "--url",
                               "http://test.url:8089",
                               "--username",
                               "testname",
                               "--password",
                               "testpassword",
                               "--db_path",
                               "path/to/db/directory/file.db",
                               "--tmp_path",
                               "path/to/tmp/directory",
                               "--missions",
                               "mission1,mission2,mission3",
                               "--tmp_size",
                               "30"};
    Config instance(
        "test_home",
        sizeof(arguments) / sizeof(*arguments),
        const_cast<char**>(arguments));
    ASSERT_TRUE(instance.isValid());
    ASSERT_EQ("http://test.url:8089", instance.getUrl());
    ASSERT_EQ("testname", instance.getUsername());
    ASSERT_EQ("testpassword", instance.getPassword());
    std::vector<std::string> test_missions{"mission1", "mission2", "mission3"};
    ASSERT_EQ(test_missions, instance.getMissions());
    ASSERT_EQ("path/to/db/directory/file.db", instance.getDbPath());
    ASSERT_EQ("path/to/tmp/directory", instance.getTmpPath());
    ASSERT_EQ(30, instance.getCacheSize());
    ASSERT_TRUE(!instance.printHelp());
    ASSERT_TRUE(!instance.printVersion());
    ASSERT_TRUE(boost::filesystem::exists("path/to/db/directory"));
    ASSERT_TRUE(boost::filesystem::exists("path/to/tmp/directory"));
  }
  {
    const char* arguments[] = {"test-program-name",
                               "--url",
                               "http://test.url:8089",
                               "--username",
                               "testname",
                               "--password",
                               "testpassword",
                               "--missions",
                               "mission1"};
    Config instance(
        "test_home",
        sizeof(arguments) / sizeof(*arguments),
        const_cast<char**>(arguments));
    ASSERT_TRUE(instance.isValid());
    ASSERT_TRUE(!instance.printHelp());
    ASSERT_TRUE(!instance.printVersion());
    ASSERT_EQ("test_home/.db/products.db", instance.getDbPath());
    ASSERT_EQ("test_home/.cache/odata", instance.getTmpPath());
    ASSERT_EQ(10, instance.getCacheSize());
    ASSERT_TRUE(boost::filesystem::exists("test_home/.db"));
    ASSERT_TRUE(boost::filesystem::exists("test_home/.cache/odata"));
  }
  {
    const char* arguments[] = {"test-program-name", "--help"};
    Config instance(
        "test_home",
        sizeof(arguments) / sizeof(*arguments),
        const_cast<char**>(arguments));
    ASSERT_TRUE(instance.isValid());
    ASSERT_TRUE(instance.printHelp());
    ASSERT_TRUE(!instance.printVersion());
  }
  {
    const char* arguments[] = {"test-program-name", "--version"};
    Config instance(
        "test_home",
        sizeof(arguments) / sizeof(*arguments),
        const_cast<char**>(arguments));
    ASSERT_TRUE(instance.isValid());
    ASSERT_TRUE(!instance.printHelp());
    ASSERT_TRUE(instance.printVersion());
  }
}

TEST_F(ConfigTest, InvalidArgumentsTest) {
  {
    const char* unknown[] = {"test-program-name", "--c", "invalid"};
    Config instance(
        "test_home",
        sizeof(unknown) / sizeof(*unknown),
        const_cast<char**>(unknown));
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
        "test_home",
        sizeof(missing) / sizeof(*missing),
        const_cast<char**>(missing));
    ASSERT_TRUE(!instance.isValid());
    ASSERT_TRUE(!instance.printHelp());
    ASSERT_TRUE(!instance.printVersion());
  }

  {
    // db path must be file
    const char* missing[] = {"test-program-name",
                             "--url",
                             "http://test.url:8089",
                             "--username",
                             "testname",
                             "--db_path",
                             "/directory/only/"};
    Config instance(
        "test_home",
        sizeof(missing) / sizeof(*missing),
        const_cast<char**>(missing));
    ASSERT_TRUE(!instance.isValid());
    ASSERT_TRUE(!instance.printHelp());
    ASSERT_TRUE(!instance.printVersion());
  }
}

} // namespace Test
} // namespace OData
