#ifndef __LeaderConnection_hpp__
#define __LeaderConnection_hpp__

#include "lib/Thread.hpp"
#include "lib/SyncAccess.hpp"
#include "lib/socket/TCPConnection.hpp"
#include "server/ReplicaInfo.hpp"
#include "server/replication/ReplicaVector.hpp"
#include "server/profile/ProfileManager.hpp"
#include "server/notification/NotificationManager.hpp"

#include "server/config.hpp"

class LeaderConnection : public Thread {
  TCPConnection* m_connection;
  ReplicaVector& m_replicas;
  ReplicaInfo& m_info;
  ProfileManager& m_profileManager;
  NotificationManager& m_notificationManager;

  SyncAccess<bool> m_isRunningElection;

  public: 
    LeaderConnection(
      TCPConnection* connection,
      ReplicaVector& replicas,
      ReplicaInfo& info,
      ProfileManager& profileManager,
      NotificationManager& notificationManager);
    

  private: 
    // Handle REPLICAS request. Set the m_replicas vector to contain all current replicas
    void handleReplicas(std::string payload);

    // Handle NEW_REPLICA request. Add new replica info to m_replicas vector
    void handleNewReplica(std::string payload);

    // The main code that is executed when working as a replica
    void* run();
};

#endif // __LeaderConnection_hpp__