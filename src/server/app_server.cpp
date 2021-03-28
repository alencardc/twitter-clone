#include <new>
#include <string>
#include <stdio.h>
#include <stdlib.h>

#include "lib/Thread.hpp"
#include "lib/socket/TCPServer.hpp"
#include "server/ConnectionHandler.hpp"
#include "server/profile/ProfileManager.hpp"
#include "server/notification/NotificationManager.hpp"

using namespace std;

int main(int argc, char** argv) {
  if (argc != 3) {
    printf("Invalid parameters. Usage: %s <ip> <port>\n", argv[0]);
    return 0;
  }

  string ip = argv[1];
  int port = atoi(argv[2]);

  try {
    TCPServer* server = new TCPServer(ip, port);
    if (server->start() == false) {
      printf("[error] Unable to start the server!\n");
      return 1;
    }

    printf("[info] Server started! Listening at %s:%d\n", ip.c_str(), port);
    ProfileManager profileManager;
    profileManager.loadUsers();
    NotificationManager notificationManager;
    
    while(true) {
      TCPConnection* connection = server->accept();
      if (connection == NULL) {
        printf("[error] Unable to accept a connection!\n");
        continue;
      }
      // Check if should be added a try/catch for bad alloc to wrap work item
      try {
        ConnectionHandler* handler = new ConnectionHandler(connection, profileManager, notificationManager);
        handler->start();
      } catch(bad_alloc& e) {
        printf("[error] Thread creation: bad_alloc caught: %s\n", e.what());
      } catch(...) {
        printf("[error] Thread creation: unknown error.\n");
      }
    }
  } catch (bad_alloc& e) {
    printf("[error] bad_alloc caught: %s\n", e.what());
  }

  return 0;
}