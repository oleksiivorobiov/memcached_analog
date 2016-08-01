#include "gtest/gtest.h"
#include "cache.h"

TEST(cache_test, ShouldBeAbleToSetItem) {
  Cache c;
  c.set("a", "val");
  ASSERT_EQ("val", c.get("a"));
}

TEST(cache_test, ShouldBeAbleToRemoveItem) {
  Cache c;
  c.set("a", "val");
  c.remove("a");
  ASSERT_EQ("", c.get("a"));
}

TEST(cache_test, ShouldNotRemoveUnexpiredItem) {
  Cache c;
  c.set("a", "val", 50);
  std::this_thread::sleep_for(std::chrono::seconds(1));
  ASSERT_EQ("val", c.get("a"));
}

TEST(cache_test, ShouldRemoveExpiredItem) {
  Cache c;
  c.set("a", "val", 1);
  std::this_thread::sleep_for(std::chrono::seconds(2));
  ASSERT_EQ("", c.get("a"));
}
