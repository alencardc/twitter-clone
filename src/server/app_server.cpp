#include <new>
#include <string>
#include <stdio.h>
#include <stdlib.h>

#include "lib/Thread.hpp"
#include "lib/socket/UDPSocket.hpp"
#include "lib/socket/TCPServer.hpp"
#include "server/ServerHandler.hpp"
#include "server/ReplicaHandler.hpp"
#include "server/ConnectionHandler.hpp"
#include "server/profile/ProfileManager.hpp"
#include "server/notification/NotificationManager.hpp"

#include "config.hpp"

using namespace std;

int main(int argc, char** argv) {
  if (argc < 1 || argc > 2) {
    printf("Invalid parameters.\n");
    printf("Usage to start main server: %s\n", argv[0]);
    printf("Usage to start replica server: %s <port>\n", argv[0]);
    return 0;
  }

  string ip = DEFAULT_IP;
  int udpPort = DEFAULT_INTERNAL_PORT;
  bool isReplica = false;

  if (argc == 2) { // Replica port info
    udpPort = atoi(argv[1]);
    isReplica = true;
  }

  ProfileManager profileManager;
  NotificationManager notificationManager;
  if (isReplica == false)
    profileManager.loadUsers();

  try {
    UDPSocket* internalSock = UDPSocket::create();
    if (internalSock == NULL || internalSock->bind(ip, udpPort) == false) {
      printf("[error] Unable to start interal socket!\n");
      return 1;
    }
    
    while(true) {
      if (isReplica == true) {
        ReplicaHandler* replicaHandler = new ReplicaHandler(internalSock, profileManager, notificationManager);
        replicaHandler->start();
        // When it arrives here, means that this process was elected the leader
        isReplica = false;
        delete replicaHandler;
        printf("replica ended\n");
      } else { // It's the main server
        delete internalSock;
        internalSock = UDPSocket::create();
        if (internalSock == NULL || internalSock->bind(DEFAULT_IP, DEFAULT_INTERNAL_PORT) == false) {
          printf("[error] Unable to start interal socket!\n");
          return 1;
        }
        ServerHandler* serverHandler = new ServerHandler(internalSock, profileManager, notificationManager);
        serverHandler->start();

        TCPServer* serverSock = new TCPServer(ip, DEFAULT_SERVER_PORT);
        printf("[info] Server started! Listening at %s:%d\n", serverSock->ip().c_str(), serverSock->port());
        if (serverSock->start() == false) {
          printf("[error] Unable to start the server!\n");
          return 1;
        }
        while (true) {
          TCPConnection* connection = serverSock->accept();
          if (connection == NULL) {
            printf("[error] Unable to accept a connection!\n");
            continue;
          }

          try {
            ConnectionHandler* handler = new ConnectionHandler(connection, profileManager, notificationManager);
            handler->start();
          } catch(std::bad_alloc& e) {
            printf("[error] Thread creation: bad_alloc caught: %s\n", e.what());
          } catch(...) {
            printf("[error] Thread creation: unknown error.\n");
          }
        }
      }
    }
  } catch (bad_alloc& e) {
    printf("[error] bad_alloc caught: %s\n", e.what());
  }

  return 0;
}