#ifndef __SendTweetRoute_hpp__
#define __SendTweetRoute_hpp__

#include "Route.hpp"
#include "server/notification/NotificationManager.hpp"

class SendTweetRoute: public Route {
  std::string m_username;
  Packet& m_request;
  NotificationManager& m_manager;

  public:
    SendTweetRoute(std::string username, Packet& request, NotificationManager& manager);
    Packet* execute();
};

#endif // __SendTweetRoute_hpp__