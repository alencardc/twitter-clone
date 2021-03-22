#ifndef __FollowRoute_hpp__
#define __FollowRoute_hpp__

#include <string>
#include "Route.hpp"
#include "server/profile/ProfileManager.hpp"

class FollowRoute: public Route {
  std::string m_username;
  Packet& m_request;
  ProfileManager& m_manager;

  public:
    FollowRoute(std::string username, Packet& request, ProfileManager& manager);
    Packet* execute();
};

#endif // __FollowRoute_hpp__