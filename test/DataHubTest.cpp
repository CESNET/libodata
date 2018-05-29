#include "DataHub.h"

#include "FileSystemNode.h"
#include "MockConnection.h"
#include "MockStorage.h"
#include <gtest/gtest.h>

namespace OData {
namespace Test {

TEST(DataHubTest, GetDataTest) {
  MockConnection connection(10);
  DataHub instance(
      connection,
      {"TEST_PLATFORM"},
      std::make_shared<MockStorage>(),
      "/tmp/odata");
  const auto filesystem = instance.getData();
}

} // namespace Test
} // namespace OData
