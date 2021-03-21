#ifndef __SendTweetRoute_hpp__
#define __SendTweetRoute_hpp__

#include "Route.hpp"

class SendTweetRoute: public Route {
  Packet& m_request;

  public:
    SendTweetRoute(Packet& request);
    Packet* execute();
};

#endif // __SendTweetRoute_hpp__