#include <new>
#include <string>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>

#include "lib/utils/string.hpp"
#include "lib/utils/vector_pair.hpp"
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
/*
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
*/

bool isClosed(std::pair<TCPConnection*, ReplicaInfo> pair) {
  return pair.first->isClosed();
}

void removeClosedConnections(
  std::vector<std::pair<TCPConnection*, ReplicaInfo>>& replicasVector
) {
  replicasVector.erase(
    std::remove_if(replicasVector.begin(), replicasVector.end(), isClosed),
    replicasVector.end()
  );
}

std::string serialize(std::vector<ReplicaInfo> replicas) {
  std::string serial;
  if (replicas.size() == 0)
    return "";

  serial.append(replicas[0].serialize());
  for (unsigned i = 1; i < replicas.size(); i++) {
    serial.append("\n" + replicas[i].serialize());
  }

  return serial;
}

std::vector<ReplicaInfo> deserialize(std::string payload) {
  std::vector<ReplicaInfo> replicas;
  std::vector<std::string> splits = split(payload, "\n");
  for (std::string raw : splits)
    replicas.push_back(ReplicaInfo::deserialize(raw.c_str()));
  
  return replicas;
}

void sendHello(TCPConnection* conn, ReplicaInfo info) {
  Packet helloPacket = Packet(NEW_REPLICA, info.serialize().c_str());
  conn->send(&helloPacket);
}

std::vector<ReplicaInfo> receiveState(TCPConnection* conn, ReplicaInfo& info) {
  std::vector<ReplicaInfo> replicas;

  Packet* answerPacket = conn->receive();
  if (answerPacket != NULL) {
    std::string payload = answerPacket->payload();
    if (payload.length() > 0) {
      std::vector<std::string> idAndReplicas = splitFirst(payload, "\n");

      info.id = atoi(idAndReplicas[0].c_str());
      if (idAndReplicas[1].size() > 0)
        replicas  = deserialize(idAndReplicas[1]);
    }
  }

  return replicas;
}

ReplicaInfo receiveHello(TCPConnection* conn) {
  ReplicaInfo replica;
  Packet* helloPacket = conn->receive();
  if (helloPacket != NULL && helloPacket->type() == NEW_REPLICA) {
    replica = ReplicaInfo::deserialize(helloPacket->payload());
  }
  delete helloPacket;
  return replica;
}


void sendState(
  TCPConnection* conn, 
  ReplicaInfo& replica,
  std::vector<ReplicaInfo> replicasInfo
) {
  replica.id = replicasInfo.size();

  std::string payload = std::to_string(replica.id) + "\n" + serialize(replicasInfo); 
  Packet statePacket = Packet(REPLICAS, payload.c_str());
  conn->send(&statePacket);
}

int main(int argc, char** argv) {
  if (argc < 2 || argc > 3) {
    printf("Invalid parameters.\n");
    printf("Usage to start a server: %s <port>\n", argv[0]);
    printf("Usage to start a server: %s <port-replica> <port-leader>\n", argv[0]);
    return 0;
  }

  std::string ip = DEFAULT_IP;
  int rmPort = atoi(argv[1]);
  int leaderPort = atoi(argv[1]);
  bool isLeader = true;
  // TODO check if port is default port

  if (argc > 2) {
    isLeader = false;
    leaderPort = atoi(argv[2]);
    // TODO check if port is default port
  }

  ReplicaInfo myInfo = ReplicaInfo(-1, ip, rmPort);

  ProfileManager profileManager;
  NotificationManager notificationManager;
  if (isLeader == true)
    profileManager.loadUsers();
  
  try {
    TCPServer* rmServer = new TCPServer(ip, rmPort);
    if (rmServer->start() == false) {
      printf("[error] Unable to start the RM server!\n");
      return -1;
    }

    std::mutex electionMutex; // Used during election with the receivedAnswer condition variable
    std::condition_variable receivedAnswerCV; // Used to notify an ELECTION answer

    SyncAccess<bool> lostElection;
    lostElection.set(false);
    SyncAccess<bool> startElection;
    startElection.set(false);

    TCPConnection* leaderConn = NULL;
    std::vector<std::pair<TCPConnection*, ReplicaInfo>> replicasVector;
    while (true) {

      if (isLeader == true) {
        // Start notification server at DEFAULT_IP:DEFAULT_INTERNAL_PORT
      }

      if (isLeader == false && leaderConn == NULL) {
        TCPClient client;
        leaderConn = client.connect(DEFAULT_IP, leaderPort);
        if (leaderConn == NULL)
          printf("[error] Unable to connect to leader RM server.\n");
        else {
          sendHello(leaderConn, myInfo);
          std::vector<ReplicaInfo> replicas = receiveState(leaderConn, myInfo);
          
          ReplicaHandler* handler = new ReplicaHandler(
            leaderConn, myInfo, startElection, true,
            lostElection, electionMutex, receivedAnswerCV);
          handler->start();

          // Start connection with online replicas
          for (ReplicaInfo replica : replicas) {
            printf("[Replica] Tried to connect to <%s>\n", replica.serialize().c_str());

            TCPClient client;
            TCPConnection* conn = client.connect(replica.ip, replica.port);
            if (conn != NULL) {
              printf("[Replica] Connected to <%s>\n", replica.serialize().c_str());
              sendHello(conn, myInfo);
              replicasVector.push_back(std::make_pair(conn, replica));

              ReplicaHandler* handler = new ReplicaHandler(
                conn, myInfo, startElection, false,
                lostElection, electionMutex, receivedAnswerCV);
              handler->start();
            }
          }
        }
      }

      while (startElection.get() == false) {
        TCPConnection* conn = rmServer->accept(800);
        if (conn != NULL) {
          printf("[Replica] Received a new connection\n");
          removeClosedConnections(replicasVector);
          
          ReplicaInfo newReplica = receiveHello(conn);
          if (newReplica.isValid()) {
            if (isLeader == true) {
              std::vector<ReplicaInfo> replicasInfo;
              extract_second(replicasVector, replicasInfo);
              sendState(conn, newReplica, replicasInfo);
            }

            replicasVector.push_back(std::make_pair(conn, newReplica));

            std::vector<ReplicaInfo> replicas;
            extract_second(replicasVector, replicas);
            printf("%s\n", serialize(replicas).c_str());

            ReplicaHandler* handler = new ReplicaHandler(
              conn, myInfo, startElection, false,
              lostElection, electionMutex, receivedAnswerCV);
            handler->start();
          } else {
            printf("[Replica] Replicas was invalid, closing connection...\n");
            delete conn; conn = NULL;
          }
        }
      }
      
      removeClosedConnections(replicasVector);

      // Do the election

      printf("I am going to send ELECTION to %ld replicas\n", replicasVector.size());
      for (auto replicaPair : replicasVector) {
        TCPConnection* conn = replicaPair.first;
        ReplicaInfo replica = replicaPair.second;
        bool isNotEqualToMe = replica.ip != myInfo.ip || replica.port != myInfo.port;
        bool isGreaterThanMe = replica.id < myInfo.id; // Greater id has less value

        if (isNotEqualToMe && isGreaterThanMe) {
          Packet electionPacket = Packet(ELECTION, myInfo.serialize().c_str());
          conn->send(&electionPacket);

          std::unique_lock<std::mutex> lock(electionMutex);
          receivedAnswerCV.wait(lock);
          lock.unlock();

          if (lostElection.get() == true)
            break;
        }
      }

      std::this_thread::sleep_for(std::chrono::seconds(2));

      // If I didn`t lost to any other replica, then I'm the new leader
      if (lostElection.get() == false) { 
        printf("[Replica] I'm the new leader!\n");
        // Notify all connected replicas
        std::vector<TCPConnection*> connections;
        extract_first(replicasVector, connections);
        for (TCPConnection* conn : connections) {
          Packet coordPacket = Packet(COORDINATOR, myInfo.serialize().c_str());
          conn->send(&coordPacket);
        }

        isLeader = true;
        delete leaderConn; leaderConn = NULL;
        startElection.set(false);
      }

      lostElection.set(false);
    }
  } catch (std::bad_alloc& e) {
    printf("[error] bad_alloc caught: %s\n", e.what());
  }
  /*
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
  */
  return 0;
}