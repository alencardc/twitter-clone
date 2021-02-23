#include <iostream>
#include "lib/socket/TCPClient.hpp"

int main() {
  std::cout << "Hello, World!" << std::endl;

  TCPClient client = TCPClient();
  TCPConnection* handle = client.connect("127.0.0.1", 8080);
  std::cout << "Connected!\n";
  std::string msg = "Client saying hi to server at port 8080";
  handle->send(msg.c_str(), msg.size());

  delete handle;

  return 0;
}