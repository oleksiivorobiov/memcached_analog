#ifndef CACHE_H
#define CACHE_H

#include <unordered_map>
#include <map>
#include <mutex>
#include <thread>
#include <condition_variable>

class CacheInterface
{
public:
  virtual void set(const std::string &key, const std::string &val, unsigned int ttl_sec = 0) = 0;
  virtual std::string get(const std::string &key) = 0;
  virtual void remove(const std::string &key) = 0;
  virtual void save(const std::string &file_path) = 0;
};

class Cache : public CacheInterface
{
  class Invalidator
  {
    Cache *_cache;
    bool _stop;
    std::thread _worker;
    std::map<std::chrono::system_clock::time_point, std::string> _expirations;
    std::condition_variable _wakeup;
    std::mutex _mutex;
    static int get_curr_ttl_sec(const std::chrono::system_clock::time_point &expiration);
  public:
    Invalidator(Cache &c);
    ~Invalidator();
    void track(const std::string &key, unsigned int ttl_sec);
    void stop();
  };
  Invalidator _invalidator;
  std::unordered_map<std::string, std::string> _data;
  std::mutex _mutex;
public:
  Cache();
  ~Cache();
  void set(const std::string &key, const std::string &val, unsigned int ttl_sec = 0) override;
  std::string get(const std::string &key) override;
  void remove(const std::string &key) override;
  void save(const std::string &file_path) override;
};

#endif // CACHE_H
