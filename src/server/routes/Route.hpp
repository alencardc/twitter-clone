#ifndef __Route_hpp__
#define __Route_hpp__

#include "lib/packet/Packet.hpp"

class Route {
  public: 
    virtual Packet* execute() = 0;
};

#endif // __Route_hpp__