#include "gtest/gtest.h"
#include "cache.h"
#include <fstream>

class CacheTest: public ::testing::Test
{
public:
  Cache c;
};

TEST_F(CacheTest, ShouldBeAbleToSetItem) {
  c.set("a", "val");
  ASSERT_EQ("val", c.get("a"));
}

TEST_F(CacheTest, ShouldBeAbleToRemoveItem) {
  c.set("a", "val");
  c.remove("a");
  ASSERT_EQ("", c.get("a"));
}

TEST_F(CacheTest, ShouldNotRemoveUnexpiredItem) {
  c.set("a", "val", 50);
  std::this_thread::sleep_for(std::chrono::seconds(1));
  ASSERT_EQ("val", c.get("a"));
}

TEST_F(CacheTest, ShouldRemoveExpiredItem) {
  c.set("a", "val", 1);
  std::this_thread::sleep_for(std::chrono::seconds(2));
  ASSERT_EQ("", c.get("a"));
}

// TODO: move to test helper
std::string read_file(const std::string &path)
{
  std::ifstream in(path);
  std::string str((std::istreambuf_iterator<char>(in)),
                   std::istreambuf_iterator<char>());
  return str;
}

TEST_F(CacheTest, ShouldSaveToFile) {
  std::string path("/tmp/cache_test_ShouldSaveToFile");
  c.set("a", "val");
  c.save(path);
  ASSERT_EQ("a: val\n", read_file(path));

  c.remove("a");
  c.save(path);
  ASSERT_EQ("", read_file(path));
}
