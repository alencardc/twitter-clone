#ifndef __CONNECTION_HANDLER_HPP__
#define __CONNECTION_HANDLER_HPP__

#include <string>
#include <utility>
#include "lib/Thread.hpp"
#include "lib/socket/TCPConnection.hpp"
#include "server/profile/ProfileManager.hpp"
#include "server/notification/NotificationManager.hpp"

class ConnectionHandler : public Thread {
  TCPConnection* m_connection;
  ProfileManager& m_profileManager;
  NotificationManager& m_notificationManager;

  public: 
    ConnectionHandler(
      TCPConnection* connection,
      ProfileManager& profileManager,
      NotificationManager& notificationManager);

  private: 
    void* run();

    std::pair<std::string, long unsigned int> authorizeSession();

};


#endif // __CONNECTION_HANDLER_HPP__