#include "cache.h"
#include <fstream>

int Cache::Invalidator::get_curr_ttl_sec(const std::chrono::system_clock::time_point &expiration)
{
  return std::chrono::duration_cast<std::chrono::seconds>(expiration - std::chrono::system_clock::now()).count();
}

Cache::Invalidator::Invalidator(Cache &c) : _cache(&c), _stop(false), _worker([&]()
  {
    auto sleep_sec = 0;
    while (true)
    {
      std::unique_lock<std::mutex> lock(_mutex);
      _wakeup.wait_for(lock, std::chrono::seconds(sleep_sec));
      if (_stop)
        return;

      auto it = _expirations.begin();
      while (it != _expirations.end())
      {
        int curr_ttl_sec = get_curr_ttl_sec(it->first);
        if (curr_ttl_sec <= 0)
        {
          printf("removed\n");
          _cache->remove(it->second);
          it = _expirations.erase(it);
        }
        else
        {
          printf("skipped sleep for %d\n", curr_ttl_sec);
          sleep_sec = curr_ttl_sec;
          // next items are not expired yet, cause map items are sorted
          break;
        }
      }
    }
  })
{
}

Cache::Invalidator::~Invalidator()
{
  stop();
}

void Cache::Invalidator::track(const std::string &key, unsigned int ttl_sec)
{
  auto expire_at = std::chrono::system_clock::now() + std::chrono::seconds(ttl_sec);
  std::unique_lock<std::mutex> lock(_mutex);
  _expirations[expire_at] = key;
  lock.unlock();
  _wakeup.notify_one();
}

void Cache::Invalidator::stop()
{
  if (_stop)
    return;

  std::unique_lock<std::mutex> lock(_mutex);
  _stop = true;
  lock.unlock();
  _wakeup.notify_one();
  _worker.join();
}

Cache::Cache(std::shared_ptr<StorageInterface> storage) : _storage(storage), _invalidator(*this)
{

}

Cache::~Cache()
{
  _invalidator.stop();
}

void Cache::set(const std::string &key, const std::string &val, unsigned int ttl_sec)
{
  std::unique_lock<std::mutex> lock(_mutex);
  _storage->set(key, val);
  lock.unlock();

  if (ttl_sec > 0)
    _invalidator.track(key, ttl_sec);
}

std::string Cache::get(const std::string &key)
{
  std::lock_guard<std::mutex> lock(_mutex);
  return _storage->get(key);
}

void Cache::remove(const std::string &key)
{
  std::lock_guard<std::mutex> lock(_mutex);
  _storage->remove(key);
}

void Cache::save(const std::string &file_path)
{
  std::ofstream out(file_path);

  std::lock_guard<std::mutex> lock(_mutex);
  _storage->traverse([&](const std::string &key, const std::string &val)
  {
    out << key << ": " << val << "\n";
  });
}
