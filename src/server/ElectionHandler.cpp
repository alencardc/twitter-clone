#include <chrono>
#include "lib/packet/Packet.hpp"
#include "lib/socket/UDPSocket.hpp"
#include "server/ReplicaInfo.hpp"
#include "ElectionHandler.hpp"

ElectionHandler::ElectionHandler(
  ReplicaInfo ownerInfo, 
  std::vector<ReplicaInfo> replicas
): 
  m_ownerInfo(ownerInfo),
  m_replicas(replicas)
{}

void* ElectionHandler::run() {
  UDPSocket* socket = UDPSocket::create();

  bool gotAnAnswer = false;
  Packet electionPacket = Packet(ELECTION, "");

  // Ask replicas the known replicas to start election
  for (ReplicaInfo replica : m_replicas) {
    bool isNotEqualToMe = replica.ip != socket->ip() || replica.port != socket->port();
    bool isGreaterThanMe = replica.id > m_ownerInfo.id;
    if (isNotEqualToMe && isGreaterThanMe) {
      bool sent = socket->sendTo(&electionPacket, replica.ip, replica.port);
      if (sent == true) {
        gotAnAnswer = true;
        break;
      }
    }
  }

  // If nobody answered my election request, then I'm the leader'
  if (gotAnAnswer == false) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    Packet leaderPacket(NEW_LEADER, "");
    socket->sendTo(&leaderPacket, m_ownerInfo.ip, m_ownerInfo.port);

    Packet coordPacket = Packet(COORDINATOR, "");
    for (ReplicaInfo replica : m_replicas) {
      bool isNotEqualToMe = replica.ip != socket->ip() || replica.port != socket->port();
      if (isNotEqualToMe)
        socket->sendTo(&coordPacket, replica.ip, replica.port);
    }
  }

  return NULL;
}