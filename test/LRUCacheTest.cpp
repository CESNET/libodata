#include "LRUCache.h"

#include <gtest/gtest.h>
#include <string>

namespace OData {
namespace Test {

TEST(LRUCacheTest, PutGetTest) {
  LRUCache<std::string, int> instance(3);
  ASSERT_EQ(false, instance.get("x").is_initialized());
  instance.put("x", 1);
  ASSERT_EQ(true, instance.get("x").is_initialized());
  ASSERT_EQ(1, instance.get("x").value());

  // make cache full
  instance.put("y", 2);
  instance.put("z", 3);

  // update x to be the last to remove
  instance.put("x", 4);
  ASSERT_EQ(true, instance.get("x").is_initialized());
  ASSERT_EQ(true, instance.get("y").is_initialized());
  ASSERT_EQ(true, instance.get("z").is_initialized());
  ASSERT_EQ(4, instance.get("x").value());

  instance.put("w", 5);
  ASSERT_EQ(false, instance.get("y").is_initialized());
  ASSERT_EQ(true, instance.get("x").is_initialized());

  // make the same for put
  ASSERT_EQ(true, instance.get("x").is_initialized());
  ASSERT_EQ(true, instance.get("z").is_initialized());
  ASSERT_EQ(true, instance.get("w").is_initialized());
  instance.put("x", 5);

  instance.put("v", 5);
  ASSERT_EQ(false, instance.get("z").is_initialized());
  ASSERT_EQ(true, instance.get("x").is_initialized());
}

TEST(LRUCacheTest, RemoveTest) {
  LRUCache<std::string, int> instance(3);
  instance.put("x", 1);
  instance.put("y", 2);
  instance.put("z", 3);

  ASSERT_EQ(true, instance.get("x").is_initialized());
  instance.remove("x");
  ASSERT_EQ(false, instance.get("x").is_initialized());

  instance.put("w", 4);
  ASSERT_EQ(true, instance.get("y").is_initialized());
  ASSERT_EQ(true, instance.get("z").is_initialized());
  ASSERT_EQ(true, instance.get("w").is_initialized());
}

} // namespace Test
} // namespace OData
