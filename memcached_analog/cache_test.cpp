#include "gtest/gtest.h"
#include "cache.h"
#include <fstream>

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

std::string read_file(const std::string &path)
{
  std::ifstream in(path);
  std::string str((std::istreambuf_iterator<char>(in)),
                   std::istreambuf_iterator<char>());
  return str;
}

TEST(cache_test, ShouldSaveToFile) {
  std::string path("/tmp/cache_test_ShouldSaveToFile");
  Cache c;
  c.set("a", "val");
  c.save(path);
  ASSERT_EQ("a: val\n", read_file(path));

  c.remove("a");
  c.save(path);
  ASSERT_EQ("", read_file(path));
}
