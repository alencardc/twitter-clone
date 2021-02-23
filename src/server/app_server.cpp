#include <iostream>

#include "lib/Thread.hpp"
#include "lib/socket/TCPServer.hpp"

class TestThread : public Thread {
  private:
    void* run() {
      std::cout << "Hello Thread" << std::endl;
      return NULL;
    }
};

int main() {
  std::cout << "Hello, World!" << std::endl;
  TestThread* thread1 = new TestThread();
  thread1->start();
  thread1->join();

  TCPServer server("127.0.0.1", 8080);
  if (server.start() == false) {
    std::cout << "Unable to start server!\n";
    return 0;
  }

  TCPConnection* handle = server.accept();
  if (handle == NULL) {
    std::cout << "Unable to accept request!\n";
    return 0;
  }

  char buffer[256];
  int dataSize = handle->receive(buffer, sizeof(buffer));
  std::cout << dataSize << " bytes received!\n";
  buffer[dataSize] = '\0';
  std::cout << "Received: " << std::string(buffer) << std::endl;

  delete handle;

  return 0;
}