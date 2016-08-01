#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <string>
#include "cache.h"

class Protocol
{
  struct Command
  {
    std::string cmd, key, val;
    unsigned ttl;
    Command(const std::string &text);
    bool is_valid() const;
  };

  std::shared_ptr<CacheInterface> _cache;
public:
  static const std::string SAVE_PATH;
  Protocol(std::shared_ptr<CacheInterface> Cache);
  std::string process(const std::string &text);
};

#endif // PROTOCOL_H
