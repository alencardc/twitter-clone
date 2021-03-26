#include <string>
#include <iostream>
#include <stdlib.h>
#include "lib/utils/string.hpp"
#include "lib/socket/TCPClient.hpp"
#include "lib/packet/Packet.hpp"
#include "ConnectionConsumer.hpp"
#include "lib/Queue.hpp"

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

    Queue<Packet*> dataQueue;
    ConnectionConsumer packetReceiver = ConnectionConsumer(*connection, dataQueue);
    packetReceiver.start();

    string line; 

    bool exit = false;
    while (exit == false) {
      auto packetOrError = dataQueue.tryRemove();
      if (packetOrError.first == true) {
        printf("[receivedOnMain] %s\n", packetOrError.second->serialize().c_str());
      }

      getline(cin, line);

      if (line.rfind("SEND ", 0) == 0) {
        Packet packet = Packet(SEND, removePrefix(line, "SEND ").c_str());
        connection->send(&packet);

        cout << "[sent]: " << line << endl;
        
      } else if (line.rfind("FOLLOW", 0) == 0) {
        Packet packet = Packet(FOLLOW, removePrefix(line, "FOLLOW ").c_str());
        connection->send(&packet);

        cout << "[sent]: " << line << endl;
      } else if (line.rfind("QUIT", 0) == 0) {
        exit = true;
      }
    }
  } else {
    printf("Unable to login: %s\n", packet->payload());
  }

  delete packet;
  delete connection;

  return 0;
}