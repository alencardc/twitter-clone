#include <string>
#include <iostream>
#include <stdlib.h>
#include "lib/socket/TCPClient.hpp"
#include "lib/packet/Packet.hpp"

using namespace std;

// "127.0.0.1", 8080

int main(int argc, char** argv) {
  if (argc != 4) {
    cout << "Invalid parameters. Usage: " << argv[0] << "<ip> <port> <username>" << endl;
  }

  string ip = argv[1];
  int port = atoi(argv[2]);
  string username = argv[3];

  TCPClient* client = new TCPClient();
  TCPConnection* connection = client->connect(ip, port);

  if (connection == NULL) {
    printf("Unable stablish a connection with the server.\n");
    return 1;
  }
  
  Packet loginPacket = Packet(LOGIN, username.c_str());
  connection->send(&loginPacket);

  Packet* packet = connection->receive();
  if (packet->type() == OK) {
    printf("Logged in as: %s\n", username.c_str());
    delete packet;
  } else {
    printf("Unable to login\n");
    delete connection;

    return 0;
  }

  string line;
  int length;
  char buff[256];

  bool exit = false;
  while (exit == false) {
    getline(cin, line);

    if (line.rfind("SEND ", 0) == 0) {
      Packet packet = Packet(SEND, line.c_str());
      connection->send(&packet);

      cout << "[sent]: " << line << endl;

      length = connection->receive(buff, sizeof(buff));
      if (length == 0) {
        printf("Connection lost. Unable to reach the server.\n");
      } else {
        buff[length] = '\0';
        cout << "[received]: " << buff << endl;
      }
    } else if (line.rfind("QUIT", 0) == 0) {
      exit = true;
    }
  }
  
  delete connection;

  return 0;
}