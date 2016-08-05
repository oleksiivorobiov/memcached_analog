#include "storage.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <cstring>

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

MmapStorage::MmapStorage(const std::string &file_path, size_t size)
{
  _size = size;
  _file_descr = open(file_path.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
  if (_file_descr == -1)
    throw std::runtime_error("failed to open " + file_path);

  _mmapped = mmap(0, _size, PROT_WRITE, MAP_SHARED, _file_descr, 0);
  if (_mmapped == MAP_FAILED)
  {
    close(_file_descr);
    throw std::runtime_error("failed to mmap " + file_path);
  }
  lseek (_file_descr, _size, SEEK_SET);
  write (_file_descr, "", 1);
  lseek (_file_descr, 0, SEEK_SET);
  close(_file_descr);

  load_from_file();
}

MmapStorage::~MmapStorage()
{
  sync_mem_and_file();
  munmap(_mmapped, _size);
}

void MmapStorage::set(const std::string &key, const std::string &val)
{
  _mem_storage.set(key, val);
}

std::string MmapStorage::get(const std::string &key)
{
  return _mem_storage.get(key);
}

void MmapStorage::remove(const std::string &key)
{
  _mem_storage.remove(key);
}

void MmapStorage::traverse(std::function<void(const std::string &key, const std::string &val)> callback) const
{
  _mem_storage.traverse(callback);
}

void MmapStorage::sync_mem_and_file() const
{
  size_t offset = 0;
  memset(_mmapped, 0, _size);
  _mem_storage.traverse([&](const std::string &key, const std::string &val)
  {
    offset += sprintf((char*)_mmapped + offset, "%s\n%s\n", key.c_str(), val.c_str());
  });
  msync(_mmapped, _size, MS_SYNC);
}

void MmapStorage::load_from_file()
{
  std::string key;
  const void *mem_begin = _mmapped;
  while (true)
  {
    const void *mem_end = memchr(mem_begin, '\n', _size);
    if (!mem_end)
      break;
    std::string s((char*)mem_begin, (char*)mem_end - (char*)mem_begin);
    if (key.empty())
      key = std::move(s);
    else
    {
      _mem_storage.set(key, s);
      key.clear();
    }
    mem_begin = (char*)mem_end + 1;
  }
}
