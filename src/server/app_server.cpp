#include <new>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>

#include "lib/Thread.hpp"
#include "lib/SyncAccess.hpp"
#include "lib/socket/UDPSocket.hpp"
#include "lib/socket/TCPServer.hpp"
#include "lib/socket/TCPClient.hpp"
//#include "server/ServerHandler.hpp"
#include "server/replication/ReplicaVector.hpp"
#include "server/replication/rm/LeaderHandler.hpp"
#include "server/replication/rm/ReplicaHandler.hpp"
#include "server/replication/rm/LeaderConnection.hpp"
#include "server/ConnectionHandler.hpp"
#include "server/profile/ProfileManager.hpp"
#include "server/notification/NotificationManager.hpp"

#include "config.hpp"

void handleReplica(TCPServer* server, ProfileManager& profile, NotificationManager& notification) {
  ReplicaVector replicas;
  ReplicaInfo info = ReplicaInfo(-1, server->ip(), server->port());
  SyncAccess<bool> shouldStop;
  shouldStop.set(false);
  SyncAccess<bool> isElecting;
  isElecting.set(false);

  TCPClient client;
  TCPConnection* leaderConn = client.connect(DEFAULT_IP, DEFAULT_INTERNAL_PORT);
  LeaderConnection* leaderConnHandler = new LeaderConnection(leaderConn, replicas, info, isElecting, profile, notification);
  leaderConnHandler->start();

  std::vector<ReplicaHandler*> handlers;
  while (shouldStop.get() == false) {
    TCPConnection* connection = server->accept(800);
    if (connection == NULL && shouldStop.get() == true) {
      for (unsigned i = 0; i < handlers.size(); i++) {
        handlers[i]->join();
        delete handlers[i];
      }
      return;
    } 

    if (connection == NULL)
      continue;

    try {
      ReplicaHandler* handler = new ReplicaHandler(connection, info, replicas, isElecting);
      handler->start();
      handlers.push_back(handler);
    } catch(std::bad_alloc& e) {
      printf("[error] Thread creation: bad_alloc caught: %s\n", e.what());
    } catch(...) {
      printf("[error] Thread creation: unknown error.\n");
    }
  }
}

void handleLeader(TCPServer* server, ProfileManager& profile, NotificationManager& notification) {
  SyncAccess<bool> shouldStop;
  shouldStop.set(false);

  ReplicaVector replicas;
  std::vector<LeaderHandler*> handlers;
  while (shouldStop.get() == false) {
    TCPConnection* connection = server->accept(800);
    if (connection == NULL && shouldStop.get() == true) {
      for (unsigned i = 0; i < handlers.size(); i++) {
        handlers[i]->join();
        delete handlers[i];
      }
      return;
    }

    if (connection == NULL)
      continue;

    try {
      LeaderHandler* handler = new LeaderHandler(connection, replicas, profile, notification);
      handler->start();
      handlers.push_back(handler);
    } catch(std::bad_alloc& e) {
      printf("[error] Thread creation: bad_alloc caught: %s\n", e.what());
    } catch(...) {
      printf("[error] Thread creation: unknown error.\n");
    }
  }
}

int main(int argc, char** argv) {
  if (argc < 1 || argc > 2) {
    printf("Invalid parameters.\n");
    printf("Usage to start main server: %s\n", argv[0]);
    printf("Usage to start replica server: %s <port>\n", argv[0]);
    return 0;
  }

  std::string ip = DEFAULT_IP;
  int rmPort = DEFAULT_INTERNAL_PORT;
  bool isReplica = false;

  if (argc == 2) { // Replica port info
    rmPort = atoi(argv[1]);
    isReplica = true;
  }

  ProfileManager profileManager;
  NotificationManager notificationManager;
  if (isReplica == false)
    profileManager.loadUsers();

  try {
    while(true) {
      if (isReplica == true) {
        TCPServer* rmSocket = new TCPServer(ip, rmPort);
        if (rmSocket->start() == false) {
          printf("[error] Unable to start the RM server!\n");
          return 1;
        }
        printf("[info] RM Server started! Listening at %s:%d\n", rmSocket->ip().c_str(), rmSocket->port());

        handleReplica(rmSocket, profileManager, notificationManager);
        // When it arrives here, means that this process was elected the leader
        isReplica = false;

      } else { // It's the main server
        TCPServer* rmSocket = new TCPServer(DEFAULT_IP, DEFAULT_INTERNAL_PORT);
        if (rmSocket->start() == false) {
          printf("[error] Unable to start the Leader RM server!\n");
          return 1;
        }
        printf("[info] Leader RM Server started! Listening at %s:%d\n", rmSocket->ip().c_str(), rmSocket->port());

        handleLeader(rmSocket, profileManager, notificationManager);
        // Will not arrive here
        //ServerHandler* serverHandler = new ServerHandler(internalSock, profileManager, notificationManager);
        //serverHandler->start();

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
  } catch (std::bad_alloc& e) {
    printf("[error] bad_alloc caught: %s\n", e.what());
  }

  return 0;
}