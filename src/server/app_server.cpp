#include <string>
#include "server/replication/ReplicaManager.hpp"

#include "config.hpp"


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

  if (argc > 2) {
    isLeader = false;
    leaderPort = atoi(argv[2]);
  }

  ReplicaInfo myInfo = ReplicaInfo(-1, ip, rmPort);
  ReplicaManager rm(
    myInfo, DEFAULT_IP, DEFAULT_SERVER_PORT,
    isLeader, DEFAULT_IP, leaderPort
  );
  rm.start();
  
  return 0;
}