#include "gtest/gtest.h"
#include "asio.hpp"
#include "server.h"
#include "protocol.h"
#include "cache.h"

const std::string MMAP_STORAGE = "/tmp/memcached_analog",
                  SAVE_PATH_USR1 = "/tmp/memcached_analog_usr1";
const size_t MMAP_SIZE = 1024 * 1024;

std::shared_ptr<MmapStorage> storage;
std::shared_ptr<Cache> cache;
std::unique_ptr<ServerInterface> server;
struct sigaction act;

static void sighandler(int signo, siginfo_t *, void *)
{
  switch (signo) {
    case SIGUSR1:
      cache->save(SAVE_PATH_USR1);
      std::cout << "saved cached data in " << SAVE_PATH_USR1 << "\n";
      break;
    default:
      // we want to make sure data is written on the disk
      // destructor may not called due to detached threads use shared_ptr
      storage->sync_mem_and_file();
      server->stop();
      break;
    }
}

static void listen_signals()
{
  memset(&act, 0, sizeof(act));
  act.sa_sigaction = sighandler;
  sigaction(SIGINT, &act, NULL);
  sigaction(SIGHUP, &act, NULL);
  sigaction(SIGUSR1, &act, NULL);
}

static int run_server(int argc, char **argv)
{
  try
  {
    if (argc != 2)
    {
      std::cerr << "Usage: " << argv[0] << " <port>\n";
      return 1;
    }
    short port = std::atoi(argv[1]);
    listen_signals();

    storage = std::make_shared<MmapStorage>(MMAP_STORAGE, MMAP_SIZE);
    cache = std::make_shared<Cache>(storage);
    Protocol protocol(cache);

    std::cout << "server listen localhost on port " << port << "\n"
      << "accepted commands:\n"
      << "set <key> <val> [<time to live in sec>]\n"
      << "get <key>\n"
      << "remove <key>\n";

    server.reset(new ThreadedServer(port, protocol));
    //server.reset(new AsyncServer(port, protocol));
    server->start();
  }
  catch (const std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}

static int run_tests(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

int main(int argc, char **argv)
{
  if (argc > 1 && strcmp(argv[1], "test") == 0)
    return run_tests(argc, argv);

  return run_server(argc, argv);
}
