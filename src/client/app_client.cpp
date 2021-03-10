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
  string msg;
  char buff[256];
  if (connection) {
    msg = "Hello, how are you doing? I'm a TCP client!";
    connection->send(msg.c_str(), msg.size());
    cout << "[sent]: " << msg << endl;

    length = connection->receive(buff, sizeof(buff));
    buff[length] = '\0';
    cout << "[received]: " << buff << endl;

    delete connection;
  }

  connection = client->connect(ip, port);
  if (connection) {
    msg = "Nice to meet you again!";
    connection->send(msg.c_str(), msg.size());
    cout << "[sent]: " << msg << endl;

    length = connection->receive(buff, sizeof(buff));
    buff[length] = '\0';
    cout << "[received]: " << buff << endl;
    
    delete connection;
  }

  return 0;
}