#ifndef __ServerHandler_hpp__
#define __ServerHandler_hpp__

#include "lib/Thread.hpp"
#include "server/profile/ProfileManager.hpp"
#include "server/notification/NotificationManager.hpp"

#include "server/config.hpp"

class ServerHandler : public Thread {
  std::string m_ip;
  int m_port;
  ProfileManager& m_profileManager;
  NotificationManager& m_notificationManager;


  public: 
    ServerHandler(
      std::string ip, 
      int port, ProfileManager& profileManager,
      NotificationManager& notificationManager
    );

  private: 
    // The main code that is executed when handling new client connections (frontend)
    void* run();
};

#endif // __ServerHandler_hpp__