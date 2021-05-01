#ifndef __ReplicaHandler_hpp__
#define __ReplicaHandler_hpp__

#include "lib/Thread.hpp"
#include "lib/SyncAccess.hpp"
#include "lib/socket/TCPConnection.hpp"
#include "server/ReplicaInfo.hpp"
#include "server/replication/ReplicaVector.hpp"
#include "server/profile/ProfileManager.hpp"
#include "server/notification/NotificationManager.hpp"

#include "server/config.hpp"

class ReplicaHandler : public Thread {
  TCPConnection* m_connection;
  ReplicaInfo m_info;
  ReplicaVector& m_replicas;
  SyncAccess<bool>& m_isRunningElection;


  public: 
    ReplicaHandler(
      TCPConnection* connection,
      ReplicaInfo info,
      ReplicaVector& replicas,
      SyncAccess<bool>& electing
      );

  private: 
    // Handle REPLICAS request. Set the m_replicas vector to contain all current replicas
    void handleReplicas(std::string payload);

    // Handle NEW_REPLICA request. Add new replica info to m_replicas vector
    void handleNewReplica(std::string payload);

    // The main code that is executed when working as a replica
    void* run();
};

#endif // __ReplicaHandler_hpp__