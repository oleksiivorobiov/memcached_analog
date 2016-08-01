#include "gtest/gtest.h"
#include "asio.hpp"
#include "server.h"
#include "protocol.h"
#include "cache.h"

int run_server(int argc, char **argv)
{
  try
  {
    if (argc != 2)
    {
      std::cerr << "Usage: " << argv[0] << " <port>\n";
      return 1;
    }

    short port = std::atoi(argv[1]);
    auto cache = std::make_shared<Cache>();
    Protocol protocol(cache);
    //ThreadedServer server(port, protocol);
    AsyncServer server(port, protocol);
  }
  catch (const std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}

int run_tests(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

int main(int argc, char **argv) {
  if (argc > 1 && strcmp(argv[1], "test") == 0)
    return run_tests(argc, argv);

  return run_server(argc, argv);
}
