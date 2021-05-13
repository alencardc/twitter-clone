#ifndef __ReplicaHandler_hpp__
#define __ReplicaHandler_hpp__

#include "lib/Thread.hpp"
#include "lib/SyncAccess.hpp"
#include "lib/socket/TCPConnection.hpp"
#include "server/replication/ReplicaInfo.hpp"
#include "server/profile/ProfileManager.hpp"
#include "server/notification/NotificationManager.hpp"

#include "server/config.hpp"

class ReplicaHandler : public Thread {
  TCPConnection* m_connection;
  ReplicaInfo m_info;

  ProfileManager& m_profileManager;
  NotificationManager& m_notificationManager;

  SyncAccess<bool>& m_isRunningElection;
  bool m_isConnectedToLeader;
  SyncAccess<bool>& m_lostElection;
  std::mutex& m_mutex;
  std::condition_variable& m_cv;


  public: 
    ReplicaHandler(
      TCPConnection* connection,
      ReplicaInfo info,
      ProfileManager& profileManager,
      NotificationManager& notificationManager,
      SyncAccess<bool>& electing,
      bool isConnectedToLeader,
      SyncAccess<bool>& lostElection,
      std::mutex& mutex,
      std::condition_variable& cv
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