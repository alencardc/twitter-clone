#include <cstring>
#include <sstream>
#include "ReplicaHandler.hpp"
#include "AliveHandler.hpp"

ReplicaHandler::ReplicaHandler(
  UDPSocket* socket,
  ProfileManager& profileManager,
  NotificationManager& notificationManager
): 
  m_socket(socket),
  m_profileManager(profileManager),
  m_notificationManager(notificationManager)
{
  m_isRunningElection.set(false);
};

void ReplicaHandler::start() {
  if (m_socket == NULL) {
    return;
  }
  run();
}

void* ReplicaHandler::run() {
  std::string mainServerIp = DEFAULT_IP;
  int mainServerPort = DEFAULT_INTERNAL_PORT;

  AliveHandler* aliveHandler = new AliveHandler(
    m_isRunningElection,
    m_socket->ip(),
    m_socket->port()
  );
  aliveHandler->start();

  Packet* sendPacket = new Packet(NEW_REPLICA, "");
  m_socket->sendTo(sendPacket, mainServerIp, mainServerPort);
  delete sendPacket; sendPacket = NULL;

  while (true) {
    std::string senderIp;
    int senderPort;
    Packet* receivedPacket = m_socket->receiveFrom(&senderIp, &senderPort, true);
    if (receivedPacket == NULL) {
      continue;
    }
    switch (receivedPacket->type()) {
      case ELECTION:
        sendPacket = new Packet(ACK, "");
        m_socket->sendTo(sendPacket, senderIp, senderPort);
        m_isRunningElection.set(true);
        printf("Run election\n");
        break;
      case NEW_REPLICA:
        handleNewReplica(receivedPacket->payload());
        break;
      case REPLICAS:
        handleReplicas(receivedPacket->payload());
        break;
      default:
        break;
    }

    delete sendPacket; sendPacket = NULL;
    delete receivedPacket; receivedPacket = NULL;
  }
  delete aliveHandler;
}

void ReplicaHandler::handleReplicas(std::string payload) {
  m_replicas.clear();
  if (payload.length() > 0) {
    std::stringstream stream;
    stream << payload;
    char buffer[30];
    stream.getline(buffer, 29 ,'\n');
    m_id = atoi(buffer);
    printf("ID: %d\n", m_id);
    while(stream.good()) {
      stream.getline(buffer, 29 ,'\n');
      if (stream.good())
        m_replicas.push_back(HostAddress::deserialize(buffer));
    }
  }
}

void ReplicaHandler::handleNewReplica(std::string payload) {
  if (payload.length() > 0) {
    std::stringstream stream;
    stream << payload;
    char buffer[30];
    stream.getline(buffer, 29 ,'\n');
    std::string strReplica = buffer;
    printf("Receibed: %s\n", strReplica.c_str());
    if (strReplica.length() > 0)
      m_replicas.push_back(HostAddress::deserialize(buffer));
  }
}