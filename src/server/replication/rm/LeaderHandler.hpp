#ifndef __LeaderHandler_hpp__
#define __LeaderHandler_hpp__

#include "lib/Thread.hpp"
#include "lib/SyncAccess.hpp"
#include "lib/socket/TCPConnection.hpp"
#include "server/ReplicaInfo.hpp"
#include "server/replication/ReplicaVector.hpp"
#include "server/profile/ProfileManager.hpp"
#include "server/notification/NotificationManager.hpp"

#include "server/config.hpp"

class LeaderHandler : public Thread {
  TCPConnection* m_connection;
  ReplicaVector& m_replicas;
  ProfileManager& m_profileManager;
  NotificationManager& m_notificationManager;

  SyncAccess<bool> m_isRunningElection;
  int m_id;

  public: 
    LeaderHandler(
      TCPConnection* connection,
      ReplicaVector& replicas,
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

#endif // __LeaderHandler_hpp__