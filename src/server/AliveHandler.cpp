#include <chrono>
#include "AliveHandler.hpp"
#include "lib/socket/UDPSocket.hpp"

#include "config.hpp"

AliveHandler::AliveHandler(SyncAccess<bool>& isLeader, SyncAccess<bool>& election, std::string ownerIp, int ownerPort):
  m_isLeader(isLeader),
  m_election(election),
  m_ownerIp(ownerIp),
  m_ownerPort(ownerPort)
{}

void* AliveHandler::run() {
  UDPSocket* socket = UDPSocket::create();

  Packet alivePacket = Packet(IS_ALIVE, "");

  while (m_isLeader.get() == false) {
    if (m_election.get() == false) {
      printf("Sent alive!\n");
      std::this_thread::sleep_for(std::chrono::seconds(ALIVE_SLEEP));
      bool wasSent = socket->sendTo(&alivePacket, DEFAULT_IP, DEFAULT_INTERNAL_PORT);
      if (wasSent == false && m_election.get() == false) { 
        // Main server is down, run election
        Packet electionPacket = Packet(ELECTION, "");
        socket->sendTo(&electionPacket, m_ownerIp, m_ownerPort);
        printf("Sent election\n");
        std::this_thread::sleep_for(std::chrono::seconds(ALIVE_SLEEP*4));
        printf("Waked up\n");
      }
    }
  }
  return NULL;
}