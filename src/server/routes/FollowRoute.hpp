#ifndef __FollowRoute_hpp__
#define __FollowRoute_hpp__

#include "Route.hpp"
#include "server/profile/ProfileManager.hpp"

class FollowRoute: public Route {
  Packet& m_request;
  ProfileManager& m_manager;

  public:
    FollowRoute(Packet& request, ProfileManager& manager);
    Packet* execute();
};

#endif // __FollowRoute_hpp__