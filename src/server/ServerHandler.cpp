#include <new>
#include <cstring>
#include "ServerHandler.hpp"

ServerHandler::ServerHandler(
  UDPSocket* socket,
  ProfileManager& profileManager,
  NotificationManager& notificationManager
): 
  m_socket(socket),
  m_profileManager(profileManager),
  m_notificationManager(notificationManager)
{};

void ServerHandler::start() {
  run();
}

void* ServerHandler::run() {
  printf("I'm the leader!\n");
  if (m_socket == NULL) {
    return NULL;
  }
  printf("For sure\n");
  Packet* answer = NULL;
  std::string senderIp;
  int senderPort;

  while (true) {
    printf("Loop\n");
    Packet* received = m_socket->receiveFrom(&senderIp, &senderPort, true);
    if (received == NULL)
      continue;
    printf("%s", received->serialize().c_str());
    switch (received->type()) {
      case NEW_REPLICA:
        printf("Got a new replica request\n");
        answer = new Packet(ACK, "");
        m_socket->sendTo(answer, senderIp, senderPort, false);
        handleNewReplica(senderIp, senderPort);
        printf("Yes\n");
        break;
      case IS_ALIVE:
         printf("I'm sure I'm alive\n");
        answer = new Packet(ACK, "");
        m_socket->sendTo(answer, senderIp, senderPort, false);
        printf("Yes\n");
        break;

      default:
        break;
    }
    printf("pre delete\n");
    delete answer; answer = NULL;
    delete received; received = NULL;
    printf("pos delete\n");
  }
  
}

void ServerHandler::handleNewReplica(std::string senderIp, int senderPort) {
  ReplicaInfo newReplica = ReplicaInfo(m_replicas.size(), senderIp, senderPort);
  // Create payload and notify replicas
  std::string payload = std::to_string(m_replicas.size()) + "\n";
  for (ReplicaInfo replica : m_replicas) {
    payload.append(replica.serialize() + "\n");
    // Notify other replicas of the new replica
    Packet packet = Packet(NEW_REPLICA, newReplica.serialize().c_str());
    m_socket->sendTo(&packet, replica.ip, replica.port, false);
  }

  Packet packet = Packet(REPLICAS, payload.c_str());
  m_socket->sendTo(&packet, senderIp, senderPort, false);
        
  m_replicas.push_back(newReplica);
}