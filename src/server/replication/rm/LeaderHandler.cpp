#include <cstring>
#include <sstream>
#include "LeaderHandler.hpp"
#include "lib/socket/TCPClient.hpp"

LeaderHandler::LeaderHandler(
  TCPConnection* connection,
  ReplicaVector& replicas,
  ProfileManager& profileManager,
  NotificationManager& notificationManager
): 
  m_connection(connection),
  m_replicas(replicas),
  m_profileManager(profileManager),
  m_notificationManager(notificationManager)
{
  m_isRunningElection.set(false);
};



void* LeaderHandler::run() {

  Packet* request;
  while((request = m_connection->receive()) != NULL) {
    if (request->type() == NEW_REPLICA) {
      std::vector<ReplicaInfo> oldReplicas = m_replicas.toVector();

      ReplicaInfo newReplica = ReplicaInfo::deserialize(request->payload());
      newReplica = m_replicas.createAndInsert(newReplica);
      
      std::string payload = std::to_string(newReplica.id) + "\n";
      for (ReplicaInfo replica : oldReplicas) {
        payload.append(replica.serialize() + "\n");
        // Notify other replicas of the new replica
        Packet packet = Packet(NEW_REPLICA, newReplica.serialize().c_str());
        TCPClient client;
        TCPConnection* conn = client.connect(replica.ip, replica.port);
        conn->send(&packet);
        delete conn;
      }

      Packet packet = Packet(REPLICAS, payload.c_str());
      m_connection->send(&packet);
    
    }
  }


  return NULL;
}
