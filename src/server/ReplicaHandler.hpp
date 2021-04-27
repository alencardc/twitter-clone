#ifndef __ReplicaHandler_hpp__
#define __ReplicaHandler_hpp__

#include "lib/Thread.hpp"
#include "lib/SyncAccess.hpp"
#include "lib/socket/UDPSocket.hpp"
#include "lib/socket/HostAddress.hpp"
#include "server/profile/ProfileManager.hpp"
#include "server/notification/NotificationManager.hpp"

#include "config.hpp"

class ReplicaHandler {
  UDPSocket* m_socket;
  ProfileManager& m_profileManager;
  NotificationManager& m_notificationManager;

  SyncAccess<bool> m_isRunningElection;
  int m_id;
  std::vector<HostAddress> m_replicas;

  public: 
    ReplicaHandler(
      UDPSocket* socket,
      ProfileManager& profileManager,
      NotificationManager& notificationManager);
    
    void start();

  private: 
    void handleReplicas(std::string payload);
    void handleNewReplica(std::string payload);
    void* run();
};

#endif // __ReplicaHandler_hpp__