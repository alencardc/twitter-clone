#include <cstring>
#include <sstream>
#include "ReplicaHandler.hpp"
#include "server/replication/Election.hpp"

ReplicaHandler::ReplicaHandler(
  TCPConnection* connection,
  ReplicaInfo info,
  ReplicaVector& replicas,
  SyncAccess<bool>& electing
): 
  m_connection(connection),
  m_info(info),
  m_replicas(replicas),
  m_isRunningElection(electing)
{};



void* ReplicaHandler::run() {

  

  Packet* request;
  while((request = m_connection->receive()) != NULL) {
    if (request->type() == NEW_REPLICA) {
      ReplicaInfo newReplica = ReplicaInfo::deserialize(request->payload());
      m_replicas.insert(newReplica);
      printf("Received: %s\n", newReplica.serialize().c_str());
    
    } else if (request->type() == ELECTION) {
      if (m_isRunningElection.get() == false) {
        m_isRunningElection.set(true);
        bool electedAsLeader = Election::startElection(m_info, m_replicas.toVector());
        m_isRunningElection.set(false);
      }
    }
  }
  printf("Bye!\n");
  return NULL;
}
