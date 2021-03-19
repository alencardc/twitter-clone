#include <string>
#include <iostream>
#include <stdlib.h>
#include "lib/socket/TCPClient.hpp"
#include "lib/packet/Packet.hpp"

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

  if (connection == NULL) {
    printf("Unable stablish a connection with the server.\n");
    return 1;
  }

  int length;
  string line;
  char buff[256];

  bool exit = false;
  while (exit == false) {
    getline(cin, line);

    if (line.rfind("SEND ", 0) == 0) {
      Packet* packet = new Packet(COMMAND, line.c_str());
      connection->send(packet);

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