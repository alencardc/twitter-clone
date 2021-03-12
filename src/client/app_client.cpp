#include <string>
#include <iostream>
#include <stdlib.h>
#include "lib/socket/TCPClient.hpp"

using namespace std;

// "127.0.0.1", 8080

int main(int argc, char** argv) {
  if (argc != 3) {
    cout << "Invalid parameters. Usage: " << argv[0] << "<ip> <port>" << endl;
  }

  string ip = argv[1];
  int port = atoi(argv[2]);

  TCPClient* client = new TCPClient();
  TCPConnection* connection = client->connect(ip, port);

  int length;
  string line;
  char buff[256];

  bool exit = false;
  while (exit == false) {
    getline(cin, line);

    if (line.rfind("SEND ", 0) == 0) {
      connection->send(line.c_str(), line.size());
      cout << "[sent]: " << line << endl;

      length = connection->receive(buff, sizeof(buff));
      buff[length] = '\0';
      cout << "[received]: " << buff << endl;
    } else if (line.rfind("QUIT", 0) == 0) {
      exit = true;
    }
  }
  
  delete connection;

  return 0;
}