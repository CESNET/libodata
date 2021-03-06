#include "DataHub.h"

#include "FileSystemNode.h"
#include "MockConnection.h"
#include "MockStorage.h"
#include "PathBuilder.h"
#include <chrono>
#include <gtest/gtest.h>
#include <sstream>
#include <thread>
#include <vector>

namespace OData {
namespace Test {
namespace {
const auto test_tree = "root {\n"
                       "\tTEST_PLATFORM {\n"
                       "\t\t2018-03-08 {\n"
                       "\t\t\t{\n"
                       "\t\t\t\tbeginposition=2018-03-08T10:45:22\n"
                       "\t\t\t\tfilename=TEST_FILE0\n"
                       "\t\t\t\tidentifier=TEST_NAME0\n"
                       "\t\t\t\tplatformname=TEST_PLATFORM\n"
                       "\t\t\t\tsize=1000000KB\n"
                       "\t\t\t\tuuid=TEST_UUID0\n"
                       "\t\t\t\tfiles {\n"
                       "\t\t\t\t\tTEST_NAME0.zip( 1000000000 "
                       ")^Products('TEST_UUID0')/$value\n"
                       "\t\t\t\t\tTEST_FILE0 {\n"
                       "\t\t\t\t\t\ttestfile.xml( 44088 "
                       ")^Products('TEST_UUID0')/Nodes('TEST_FILE0')/"
                       "Nodes('testfile.xml')/$value\n"
                       "\t\t\t\t\t\ttestfolder {\n"
                       "\t\t\t\t\t\t\ttestfile2.xml( 44088 "
                       ")^Products('TEST_UUID0')/Nodes('TEST_FILE0')/"
                       "Nodes('testfolder')/Nodes('testfile2.xml')/$value\n"
                       "\t\t\t\t\t\t}\n"
                       "\t\t\t\t\t}\n"
                       "\t\t\t\t\t\n"
                       "\t\t\t\t}\n"
                       "\t\t\t}\n"
                       "\t\t}\n"
                       "\t\t2018-03-09 {\n"
                       "\t\t\t{\n"
                       "\t\t\t\tbeginposition=2018-03-09T10:45:22\n"
                       "\t\t\t\tfilename=TEST_FILE1\n"
                       "\t\t\t\tidentifier=TEST_NAME1\n"
                       "\t\t\t\tplatformname=TEST_PLATFORM\n"
                       "\t\t\t\tsize=1000000KB\n"
                       "\t\t\t\tuuid=TEST_UUID1\n"
                       "\t\t\t\tfiles {\n"
                       "\t\t\t\t\tTEST_NAME1.zip( 1000000000 "
                       ")^Products('TEST_UUID1')/$value\n"
                       "\t\t\t\t\tTEST_FILE1 {\n"
                       "\t\t\t\t\t\ttestfile.xml( 44088 "
                       ")^Products('TEST_UUID1')/Nodes('TEST_FILE1')/"
                       "Nodes('testfile.xml')/$value\n"
                       "\t\t\t\t\t\ttestfolder {\n"
                       "\t\t\t\t\t\t\ttestfile2.xml( 44088 "
                       ")^Products('TEST_UUID1')/Nodes('TEST_FILE1')/"
                       "Nodes('testfolder')/Nodes('testfile2.xml')/$value\n"
                       "\t\t\t\t\t\t}\n"
                       "\t\t\t\t\t}\n"
                       "\t\t\t\t\t\n"
                       "\t\t\t\t}\n"
                       "\t\t\t}\n"
                       "\t\t}\n"
                       "\t\t2018-03-10 {\n"
                       "\t\t\t{\n"
                       "\t\t\t\tbeginposition=2018-03-10T10:45:22\n"
                       "\t\t\t\tfilename=TEST_FILE2\n"
                       "\t\t\t\tidentifier=TEST_NAME2\n"
                       "\t\t\t\tplatformname=TEST_PLATFORM\n"
                       "\t\t\t\tsize=1000000KB\n"
                       "\t\t\t\tuuid=TEST_UUID2\n"
                       "\t\t\t\tfiles {\n"
                       "\t\t\t\t\tTEST_NAME2.zip( 1000000000 "
                       ")^Products('TEST_UUID2')/$value\n"
                       "\t\t\t\t\tTEST_FILE2 {\n"
                       "\t\t\t\t\t\ttestfile.xml( 44088 "
                       ")^Products('TEST_UUID2')/Nodes('TEST_FILE2')/"
                       "Nodes('testfile.xml')/$value\n"
                       "\t\t\t\t\t\ttestfolder {\n"
                       "\t\t\t\t\t\t\ttestfile2.xml( 44088 "
                       ")^Products('TEST_UUID2')/Nodes('TEST_FILE2')/"
                       "Nodes('testfolder')/Nodes('testfile2.xml')/$value\n"
                       "\t\t\t\t\t\t}\n"
                       "\t\t\t\t\t}\n"
                       "\t\t\t\t\t\n"
                       "\t\t\t\t}\n"
                       "\t\t\t}\n"
                       "\t\t}\n"
                       "\t}\n"
                       "}\n";

const auto removed_product1_tree =
    "root {\n"
    "\tTEST_PLATFORM {\n"
    "\t\t2018-03-08 {\n"
    "\t\t\t{\n"
    "\t\t\t\tbeginposition=2018-03-08T10:45:22\n"
    "\t\t\t\tfilename=TEST_FILE0\n"
    "\t\t\t\tidentifier=TEST_NAME0\n"
    "\t\t\t\tplatformname=TEST_PLATFORM\n"
    "\t\t\t\tsize=1000000KB\n"
    "\t\t\t\tuuid=TEST_UUID0\n"
    "\t\t\t\tfiles {\n"
    "\t\t\t\t\tTEST_NAME0.zip( 1000000000 "
    ")^Products('TEST_UUID0')/$value\n"
    "\t\t\t\t\tTEST_FILE0 {\n"
    "\t\t\t\t\t\ttestfile.xml( 44088 "
    ")^Products('TEST_UUID0')/Nodes('TEST_FILE0')/"
    "Nodes('testfile.xml')/$value\n"
    "\t\t\t\t\t\ttestfolder {\n"
    "\t\t\t\t\t\t\ttestfile2.xml( 44088 "
    ")^Products('TEST_UUID0')/Nodes('TEST_FILE0')/"
    "Nodes('testfolder')/Nodes('testfile2.xml')/$value\n"
    "\t\t\t\t\t\t}\n"
    "\t\t\t\t\t}\n"
    "\t\t\t\t\t\n"
    "\t\t\t\t}\n"
    "\t\t\t}\n"
    "\t\t}\n"
    "\t\t2018-03-09 {\n"
    "\t\t}\n"
    "\t\t2018-03-10 {\n"
    "\t\t\t{\n"
    "\t\t\t\tbeginposition=2018-03-10T10:45:22\n"
    "\t\t\t\tfilename=TEST_FILE2\n"
    "\t\t\t\tidentifier=TEST_NAME2\n"
    "\t\t\t\tplatformname=TEST_PLATFORM\n"
    "\t\t\t\tsize=1000000KB\n"
    "\t\t\t\tuuid=TEST_UUID2\n"
    "\t\t\t\tfiles {\n"
    "\t\t\t\t\tTEST_NAME2.zip( 1000000000 "
    ")^Products('TEST_UUID2')/$value\n"
    "\t\t\t\t\tTEST_FILE2 {\n"
    "\t\t\t\t\t\ttestfile.xml( 44088 "
    ")^Products('TEST_UUID2')/Nodes('TEST_FILE2')/"
    "Nodes('testfile.xml')/$value\n"
    "\t\t\t\t\t\ttestfolder {\n"
    "\t\t\t\t\t\t\ttestfile2.xml( 44088 "
    ")^Products('TEST_UUID2')/Nodes('TEST_FILE2')/"
    "Nodes('testfolder')/Nodes('testfile2.xml')/$value\n"
    "\t\t\t\t\t\t}\n"
    "\t\t\t\t\t}\n"
    "\t\t\t\t\t\n"
    "\t\t\t\t}\n"
    "\t\t\t}\n"
    "\t\t}\n"
    "\t}\n"
    "}\n";

struct DataHubTest : public ::testing::Test {
  DataHubTest(std::uint32_t product_count = 3)
      : connection(product_count),
        path_builder("/${platformname}/${date}"),
        data_hub(
            connection,
            {"TEST_PLATFORM"},
            std::make_shared<MockStorage>(),
            "/tmp/odata",
            10,
            path_builder,
            5) {
    while (connection.getProductsListed() < product_count) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }

  void SetUp() override {
  }

  void TearDown() override {
  }
  MockConnection connection;
  PathBuilder path_builder;
  DataHub data_hub;
};
} // namespace

TEST_F(DataHubTest, GetDataTest) {
  {
    const auto filesystem = data_hub.getData();
    std::stringstream str;
    filesystem->toString(str);
    ASSERT_EQ(test_tree, str.str());
  }

  connection.addDeletedProduct(1);
  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  {
    const auto filesystem = data_hub.getData();
    std::stringstream str;
    filesystem->toString(str);
    ASSERT_EQ(removed_product1_tree, str.str());
  }
}

TEST_F(DataHubTest, GetFileTest) {
  ASSERT_EQ(nullptr, data_hub.getFile(""));
  ASSERT_EQ(data_hub.getData(), data_hub.getFile("/"));
  ASSERT_TRUE(data_hub.getFile("TEST_PLATFORM")->isDirectory());
  ASSERT_TRUE(data_hub.getFile("/TEST_PLATFORM")->isDirectory());
  ASSERT_FALSE(
      data_hub
          .getFile(
              "/TEST_PLATFORM/2018-03-08/TEST_NAME0/TEST_FILE0/testfile.xml")
          ->isDirectory());
  ASSERT_EQ(
      (std::vector<char>{'o', 'd', 'u', 'c', 't'}),
      data_hub.getFile(
          "/TEST_PLATFORM/2018-03-08/TEST_NAME0/TEST_FILE0/testfile.xml", 2, 5));
}
} // namespace Test
} // namespace OData
