#ifndef __ReplicaHandler_hpp__
#define __ReplicaHandler_hpp__

#include "lib/Thread.hpp"
#include "lib/SyncAccess.hpp"
#include "lib/socket/UDPSocket.hpp"
#include "server/ReplicaInfo.hpp"
#include "server/profile/ProfileManager.hpp"
#include "server/notification/NotificationManager.hpp"

#include "config.hpp"

class ReplicaHandler {
  UDPSocket* m_socket;
  ProfileManager& m_profileManager;
  NotificationManager& m_notificationManager;

  SyncAccess<bool> m_isRunningElection;
  int m_id;
  std::vector<ReplicaInfo> m_replicas;

  public: 
    ReplicaHandler(
      UDPSocket* socket,
      ProfileManager& profileManager,
      NotificationManager& notificationManager);
    
    // Start running as replica. Call method run()
    void start();

  private: 
    // Handle REPLICAS request. Set the m_replicas vector to contain all current replicas
    void handleReplicas(std::string payload);

    // Handle NEW_REPLICA request. Add new replica info to m_replicas vector
    void handleNewReplica(std::string payload);

    // The main code that is executed when working as a replica
    void* run();
};

#endif // __ReplicaHandler_hpp__