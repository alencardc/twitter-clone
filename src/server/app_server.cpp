#include <new>
#include <string>
#include <stdio.h>
#include <stdlib.h>

#include "lib/Thread.hpp"
#include "lib/socket/TCPServer.hpp"
#include "server/WorkItem.hpp"
#include "server/ConnectionHandler.hpp"

using namespace std;

int main(int argc, char** argv) {
  if (argc != 3) {
    printf("Invalid parameters. Usage: %s <ip> <port>\n", argv[0]);
    return 0;
  }

  string ip = argv[1];
  int port = atoi(argv[2]);

  try {
    // WorkQueue<WorkItem*> queue;
    // for (int i = 0; i < N_WORKERS; i++) {
    //   ConnectionHandler* handler = new ConnectionHandler(queue);
    //   handler->start();
    // }

    TCPServer* server = new TCPServer(ip, port);
    if (server->start() == false) {
      printf("Unable to start the server!\n");
      return 1;
    }

    while(true) {
      TCPConnection* connection = server->accept();
      if (connection == NULL) {
        printf("Unable to accept a connection!\n");
        continue;
      }
      // Check if should be added a try/catch for bad alloc to wrap work item
      try {
        ConnectionHandler* handler = new ConnectionHandler(connection);
        handler->start();
      } catch(bad_alloc& e) {
        printf("Thread creation: bad_alloc caught: %s\n", e.what());
      } catch(...) {
        printf("Thread creation: unknown error.\n");
      }
    }
  } catch (bad_alloc& e) {
    printf("bad_alloc caught: %s\n", e.what());
  }

  return 0;
}