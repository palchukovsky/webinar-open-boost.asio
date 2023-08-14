
#include "client.hpp"
#include "settings.hpp"

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/write.hpp>

#include <iostream>
#include <thread>

namespace asio = boost::asio;
using tcp = asio::ip::tcp;

namespace {

std::string requestClientName();
std::string requestLogRecord();

asio::awaitable<void> sendLogRecords(tcp::socket socket) {
  auto name = requestClientName();
  name += "\n";
  co_await asio::async_write(socket, asio::buffer(name), asio::use_awaitable);

  for (;;) {
    auto record = requestLogRecord();
    record += "\n";

    std::cout << std::this_thread::get_id() << " Sending..." << std::endl;

    co_await asio::async_write(socket, asio::buffer(record),
                               asio::use_awaitable);
  }
}

void runClient() {
  asio::io_context ioContext;

  asio::signal_set signals{ioContext, SIGINT, SIGTERM};
  signals.async_wait([&](auto, auto) { ioContext.stop(); });

  tcp::socket socket{ioContext};
  socket.connect(tcp::endpoint(asio::ip::make_address("127.0.0.1"), port));

  std::cout << std::this_thread::get_id() << " Connected..." << std::endl;

  asio::co_spawn(ioContext, sendLogRecords(std::move(socket)), asio::detached);

  ioContext.run();
}

std::string requestClientName() {
  std::cout << "Enter name of the log client: ";
  std::string name;
  std::getline(std::cin, name);
  return name;
}

std::string requestLogRecord() {
  std::cout << "Enter log message: ";
  std::string name;
  std::getline(std::cin, name);
  return name;
}

}  // namespace

auto main() -> int {
  try {
    runClient();
    return EXIT_SUCCESS;
  } catch (const std::exception& ex) {
    std::cerr << "Fatal error \"" << ex.what() << "\"." << std::endl;
  } catch (...) {
    std::cerr << "Fatal UNKNOWN error." << std::endl;
  }
  return EXIT_FAILURE;
}
