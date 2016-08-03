#ifndef STORAGE_H
#define STORAGE_H

#include <unordered_map>
#include <functional>

class StorageInterface
{
public:
  virtual ~StorageInterface() {}
  virtual void set(const std::string &key, const std::string &val) = 0;
  virtual std::string get(const std::string &key) = 0;
  virtual void remove(const std::string &key) = 0;
  virtual void traverse(std::function<void(const std::string &key, const std::string &val)> callback) const = 0;
};

class MemoryStorage : public StorageInterface
{
  std::unordered_map<std::string, std::string> _map;
public:
  void set(const std::string &key, const std::string &val) override;
  std::string get(const std::string &key) override;
  void remove(const std::string &key) override;
  void traverse(std::function<void(const std::string &key, const std::string &val)> callback) const override;
};

class MmapStorage : public StorageInterface
{
  size_t _size;
  int _file_descr;
  void *_mmapped;
  MemoryStorage _mem_storage;
  void sync_mem_and_file() const;
  void load_from_file();
public:
  MmapStorage(const std::string &file_path, size_t size);
  ~MmapStorage() override;
  void set(const std::string &key, const std::string &val) override;
  std::string get(const std::string &key) override;
  void remove(const std::string &key) override;
  void traverse(std::function<void(const std::string &key, const std::string &val)> callback) const override;
};

#endif // STORAGE_H
