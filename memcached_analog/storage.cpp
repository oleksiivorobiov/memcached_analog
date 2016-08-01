#include "storage.h"

void MemoryStorage::set(const std::string &key, const std::string &val)
{
  _map[key] = val;
}

std::string MemoryStorage::get(const std::string &key)
{
  return _map[key];
}

void MemoryStorage::remove(const std::string &key)
{
  _map.erase(key);
}

void MemoryStorage::traverse(std::function<void(const std::string &key, const std::string &val)> callback) const
{
  for (auto it = _map.begin(); it != _map.end(); ++it)
    callback(it->first, it->second);
}
