#include "protocol.h"
#include <regex>
#include <iostream>



Protocol::Command::Command(const std::string &text)
{
  ttl = 0;
  std::smatch matched;

  std::regex key_expr("^\\s*(get|remove)\\s+(\\S+)\\s*$");
  if (std::regex_match(text, matched, key_expr))
  {
    cmd = matched[1];
    key = matched[2];
    return;
  }

  std::regex set_expr("^\\s*(set)\\s+(\\S+)\\s+(\\S+)(\\s+\\d+)?\\s*$");
  if (std::regex_match(text, matched, set_expr))
  {
    cmd = matched[1];
    key = matched[2];
    val = matched[3];
    ttl = std::atoi(std::string(matched[4]).c_str());
  }
}

bool Protocol::Command::is_valid() const
{
  bool valid = !cmd.empty() && !key.empty();

  if (cmd == "set")
    return valid && !val.empty();

  return valid;
}

Protocol::Protocol(std::shared_ptr<CacheInterface> cache)
{
  _cache = cache;
}

std::string Protocol::process(const std::string &text)
{
  Command cmd(text);
  if (!cmd.is_valid())
    return "invalid command";

  if (cmd.cmd == "set")
    _cache->set(cmd.key, cmd.val, cmd.ttl);
  else if (cmd.cmd == "get")
    return _cache->get(cmd.key);
  if (cmd.cmd == "remove")
    _cache->remove(cmd.key);

  return "done";
}
