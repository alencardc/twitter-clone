#ifndef __FollowRoute_hpp__
#define __FollowRoute_hpp__

#include "Route.hpp"

class FollowRoute: public Route {
  Packet& m_request;

  public:
    FollowRoute(Packet& request);
    Packet* execute();
};

#endif // __FollowRoute_hpp__