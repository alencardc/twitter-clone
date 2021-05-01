#ifndef __Election_hpp__
#define __Election_hpp__

#include <thread>
#include <chrono>
#include "server/ReplicaInfo.hpp"
#include "lib/socket/TCPClient.hpp"

namespace Election {

  bool startElection(ReplicaInfo info, std::vector<ReplicaInfo> replicas) {
    bool gotAnAnswer = false;

    for (unsigned i = replicas.size()-1; i >= 0; i++) {
      bool isNotEqualToMe = replicas[i].ip != info.ip || replicas[i].port != info.port;
      bool isGreaterThanMe = replicas[i].id > info.id;
      if (isNotEqualToMe && isGreaterThanMe) {
        TCPClient client;
        TCPConnection* conn = client.connect(replicas[i].ip, replicas[i].port);
        if (conn != NULL) { // Connection established
          Packet electionPacket = Packet(ELECTION, "");
          conn->send(&electionPacket);
          Packet* answer = conn->receive(1000);
          if (answer == NULL) {
            delete answer;
            gotAnAnswer = true;
            break;
          }
          delete answer;
        }
      }
    }

    // If nobody answered my election request, then I'm the leader'
    if (gotAnAnswer == false) {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      for (ReplicaInfo replica : replicas) {
        bool isNotEqualToMe = replica.ip != info.ip || replica.port != info.port;
        if (isNotEqualToMe) {
          TCPClient client;
          TCPConnection* conn = client.connect(replica.ip, replica.port);
          if (conn != NULL) { // Connection established
            Packet coordPacket = Packet(COORDINATOR, "");
            conn->send(&coordPacket);
            delete conn;
          }
        }
      }
    }

    return gotAnAnswer == false; // if gotAnAnswer == false than I'm the leader
  }
}

#endif // __Election_hpp__