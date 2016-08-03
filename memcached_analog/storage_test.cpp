#include "gtest/gtest.h"
#include "storage.h"
#include <cstdio>
#include <memory>

class StorageTest: public ::testing::Test
{
public:
  std::shared_ptr<MmapStorage> storage;
  static const std::string file;
  static const size_t size;

  void SetUp() override
  {
    remove(file.c_str());
    storage = std::make_shared<MmapStorage>(file, size);
  }
};

const std::string StorageTest::file("/tmp/StorageTest");
const size_t StorageTest::size = 10 * 1024;

TEST_F(StorageTest, ShouldGet) {
  storage->set("key1", "val1");
  storage->set("key2", "val2");
  ASSERT_EQ("val1", storage->get("key1"));
  ASSERT_EQ("val2", storage->get("key2"));
}

TEST_F(StorageTest, ShouldOverwrite) {
  storage->set("key", "val1");
  ASSERT_EQ("val1", storage->get("key"));
  storage->set("key", "val2");
  ASSERT_EQ("val2", storage->get("key"));
}

TEST_F(StorageTest, ShouldLoadPrevSavedData) {
  storage->set("key1", "val1");
  storage->set("key2", "val2");
  storage.reset();

  storage = std::make_shared<MmapStorage>(file, size);
  ASSERT_EQ("val1", storage->get("key1"));
  ASSERT_EQ("val2", storage->get("key2"));
}

TEST_F(StorageTest, ShouldRemove) {
  storage->set("key", "val");
  storage->remove("key");
  ASSERT_EQ("", storage->get("key"));
}
