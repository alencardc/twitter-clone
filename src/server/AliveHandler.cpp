#include <chrono>
#include "AliveHandler.hpp"
#include "lib/socket/UDPSocket.hpp"

#include "config.hpp"

AliveHandler::AliveHandler(SyncAccess<bool>& election, std::string ownerIp, int ownerPort):
  m_election(election),
  m_ownerIp(ownerIp),
  m_ownerPort(ownerPort)
{}

void* AliveHandler::run() {
  UDPSocket* socket = UDPSocket::create();

  Packet sendPacket = Packet(IS_ALIVE, "");

  // TODO Should verify if is happening a election?
  while (true) {
    std::this_thread::sleep_for(std::chrono::seconds(ALIVE_SLEEP));
    bool wasSent = socket->sendTo(&sendPacket, DEFAULT_IP, DEFAULT_INTERNAL_PORT);
    //printf("Sent alive\n");
    if (wasSent == false  && m_election.get() == false) { 
      // Main server is down, run election
      sendPacket = Packet(ELECTION, "");
      socket->sendTo(&sendPacket, m_ownerIp, m_ownerPort);
      printf("Sent election\n");
      std::this_thread::sleep_for(std::chrono::seconds(ALIVE_SLEEP));
      printf("Waked up\n");
    }
  }
}