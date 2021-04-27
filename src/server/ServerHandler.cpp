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
  if (m_socket == NULL) {
    return NULL;
  }
  Packet* answer = NULL;
  std::string senderIp;
  int senderPort;

  while (true) {
    Packet* received = m_socket->receiveFrom(&senderIp, &senderPort, true);
    if (received == NULL)
      continue;
    
    switch (received->type()) {
      case NEW_REPLICA:
        answer = new Packet(ACK, "");
        m_socket->sendTo(answer, senderIp, senderPort, false);
        {
          std::string payload = std::to_string(m_replicas.size()) + "\n";
          for (HostAddress replica : m_replicas) {
            std::string strReplica =  replica.serialize() + "\n";
            payload.append(strReplica);
            // Notify other replicas of the new replica
            answer = new Packet(NEW_REPLICA, strReplica.c_str());
            m_socket->sendTo(answer, replica.ip, replica.port, false);
          }
          //answer = new Packet(REPLICAS, payload.c_str());
          //m_socket->sendTo(answer, senderIp, senderPort, false);
        }
        m_replicas.push_back(HostAddress(senderIp, senderPort));
        break;
      case IS_ALIVE:
        answer = new Packet(ACK, "");
        m_socket->sendTo(answer, senderIp, senderPort, false);
        break;

      default:
        break;
    }

    delete answer; answer = NULL;
    delete received; received = NULL;
  }
  
}