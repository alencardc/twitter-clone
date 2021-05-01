#include <cstring>
#include <sstream>
#include "ReplicaHandler.hpp"

ReplicaHandler::ReplicaHandler(
  TCPConnection* connection,
  ReplicaVector& replicas
): 
  m_connection(connection),
  m_replicas(replicas)
{
  m_isRunningElection.set(false);
};



void* ReplicaHandler::run() {

  

  Packet* request;
  while((request = m_connection->receive()) != NULL) {
    if (request->type() == NEW_REPLICA) {
      ReplicaInfo newReplica = ReplicaInfo::deserialize(request->payload());
      m_replicas.insert(newReplica);
      printf("Received: %s\n", newReplica.serialize().c_str());
    }
  }
  printf("Bye!\n");
  return NULL;
}
