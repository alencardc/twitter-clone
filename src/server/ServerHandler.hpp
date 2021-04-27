#ifndef __ServerHandler_hpp__
#define __ServerHandler_hpp__

#include <vector>
#include "lib/Thread.hpp"
#include "lib/socket/UDPSocket.hpp"
#include "lib/socket/TCPServer.hpp"
#include "lib/socket/HostAddress.hpp"
#include "server/profile/ProfileManager.hpp"
#include "server/notification/NotificationManager.hpp"

#include "config.hpp"

class ServerHandler {
  UDPSocket* m_socket;
  ProfileManager& m_profileManager;
  NotificationManager& m_notificationManager;
  std::vector<HostAddress> m_replicas;

  public: 
    ServerHandler(
      UDPSocket* socket,
      ProfileManager& profileManager,
      NotificationManager& notificationManager);
    
    void start();

  private: 
    void* run();
};

#endif // __ServerHandler_hpp__