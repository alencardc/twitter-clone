#include <cstring>
#include <sstream>
#include "ReplicaHandler.hpp"
#include "AliveHandler.hpp"
#include "ElectionHandler.hpp"

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

  SyncAccess<bool> isLeader;
  isLeader.set(false);

  ElectionHandler* electionHandler = NULL;
  AliveHandler* aliveHandler = new AliveHandler(
    isLeader,
    m_isRunningElection,
    m_socket->ip(),
    m_socket->port()
  );
  aliveHandler->start();

  Packet* sendPacket = new Packet(NEW_REPLICA, "");
  m_socket->sendTo(sendPacket, mainServerIp, mainServerPort);
  delete sendPacket; sendPacket = NULL;

  while (isLeader.get() == false) {
    std::string senderIp;
    int senderPort;
    Packet* receivedPacket = m_socket->receiveFrom(&senderIp, &senderPort, true);
    if (receivedPacket == NULL) {
      continue;
    }
    switch (receivedPacket->type()) {
      case NEW_REPLICA:
        printf("Got a new replica!\n");
        handleNewReplica(receivedPacket->payload());
        break;

      case REPLICAS:
        printf("Got all replicas!\n");
        handleReplicas(receivedPacket->payload());
        break;

      // case SERVER_DOWN:
      //   // Answer AliveHandler
      //   printf("Server is down!\n");
      //   sendPacket = new Packet(ACK, "");
      //   m_socket->sendTo(sendPacket, senderIp, senderPort, false);
      //   isLeader.set(startElection());
      //   break;
      
      case ELECTION:
        // Answer AliveHandler
        printf("Election!\n");
        sendPacket = new Packet(ACK, "");
        m_socket->sendTo(sendPacket, senderIp, senderPort, false);
        if (m_isRunningElection.get() == false) {
          m_isRunningElection.set(true);
          printf("COUNT ME\n");
          ReplicaInfo myInfo = ReplicaInfo(m_id, m_socket->ip(), m_socket->port());
          electionHandler = new ElectionHandler(myInfo, m_replicas);
          electionHandler->start();
        }
        break;
      
      case NEW_LEADER:
        sendPacket = new Packet(ACK, "");
        m_socket->sendTo(sendPacket, senderIp, senderPort, false);
        //m_isRunningElection.set(false);
        isLeader.set(true);
        break;

      case COORDINATOR:
        //m_isRunningElection.set(true);
        printf("The new leader was announced!\n");
        sendPacket = new Packet(ACK, "");
        m_socket->sendTo(sendPacket, senderIp, senderPort, false);
        m_isRunningElection.set(false);
        break;

      default:
        break;
    }

    //delete sendPacket; sendPacket = NULL;
    delete receivedPacket; receivedPacket = NULL;
  }
  aliveHandler->join();
  delete aliveHandler;
  delete electionHandler;

  return NULL;
}

void ReplicaHandler::handleReplicas(std::string payload) {
  m_replicas.clear();
  if (payload.length() > 0) {
    std::stringstream stream;
    stream << payload;
    char buffer[30];
    stream.getline(buffer, 29 ,'\n');
    m_id = atoi(buffer);
    while(stream.good()) {
      stream.getline(buffer, 29 ,'\n');
      if (stream.good())
        m_replicas.push_back(ReplicaInfo::deserialize(buffer));
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
      m_replicas.push_back(ReplicaInfo::deserialize(buffer));
  }
}