#include <cstring>
#include <sstream>
#include "LeaderConnection.hpp"

LeaderConnection::LeaderConnection(
  TCPConnection* connection,
  ReplicaVector& replicas,
  ReplicaInfo& info,
  ProfileManager& profileManager,
  NotificationManager& notificationManager
): 
  m_connection(connection),
  m_replicas(replicas),
  m_info(info),
  m_profileManager(profileManager),
  m_notificationManager(notificationManager)
{
  m_isRunningElection.set(false);
};



void* LeaderConnection::run() {
  Packet sendPacket = Packet(NEW_REPLICA, m_info.serialize().c_str());
  m_connection->send(&sendPacket);
  
  Packet* request;
  while((request = m_connection->receive()) != NULL) {
    if (request->type() == REPLICAS) {
      std::vector<ReplicaInfo> vector;
      std::string payload = request->payload();
      if (payload.length() > 0) {
        std::stringstream stream;
        stream << request->payload();
        char buffer[30];
        stream.getline(buffer, 29 ,'\n');
        m_info.id = atoi(buffer);
        while(stream.good()) {
          stream.getline(buffer, 29 ,'\n');
          printf("REPLICAS: Received: %s\n", buffer);
          if (stream.good())
            vector.push_back(ReplicaInfo::deserialize(buffer));
        }
      }
      m_replicas.setVector(vector);
      printf("Now Im: %s\n", m_info.serialize().c_str());
    } else if (true) {

    }
  }
  printf("Leader failed!\n");
  return NULL;
}
