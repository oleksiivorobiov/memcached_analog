#include "gtest/gtest.h"
#include "protocol.h"

class CacheSpy : public CacheInterface
{
public:
  std::string cmd, key, val;
  unsigned ttl;

  void set(const std::string &key, const std::string &val, unsigned int ttl_sec = 0) override
  {
    cmd = "set";
    this->key = key;
    this->val = val;
    this->ttl = ttl_sec;
  }
  std::string get(const std::string &key) override
  {
    cmd = "get";
    this->key = key;
    this->val = "";
    this->ttl = 0;

    return "";
  }
  void remove(const std::string &key) override
  {
    cmd = "remove";
    this->key = key;
    this->val = "";
    this->ttl = 0;
  }
  void save(const std::string &) override
  {
    cmd = "save";
  }
};

class ProtocolTest: public ::testing::Test
{
public:
  std::shared_ptr<CacheSpy> spy;
  Protocol p;

  ProtocolTest() : spy(new CacheSpy), p(spy)
  {

  }
};

TEST_F(ProtocolTest, ShouldReturnInvalidCommandInCaseInvalidFormat) {
  ASSERT_EQ("invalid command", p.process(""));
  ASSERT_EQ("invalid command", p.process("set 6"));
  ASSERT_EQ("invalid command", p.process("get"));
  ASSERT_EQ("invalid command", p.process("set a "));
}

TEST_F(ProtocolTest, ShouldAcceptValid) {
  ASSERT_EQ("", p.process("get key"));
  ASSERT_EQ("get", spy->cmd);
  ASSERT_EQ("key", spy->key);

  ASSERT_EQ("done", p.process("set key val"));
  ASSERT_EQ("set", spy->cmd);
  ASSERT_EQ("key", spy->key);
  ASSERT_EQ("val", spy->val);

  ASSERT_EQ("done", p.process("set key val 60"));
  ASSERT_EQ("set", spy->cmd);
  ASSERT_EQ("key", spy->key);
  ASSERT_EQ("val", spy->val);
  ASSERT_EQ(60u, spy->ttl);

  ASSERT_EQ("done", p.process("remove key"));
  ASSERT_EQ("remove", spy->cmd);
  ASSERT_EQ("key", spy->key);
}
