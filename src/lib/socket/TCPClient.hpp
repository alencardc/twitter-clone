// It's important to note that TCPClient only means that this class work as
// a client side by actively asking to establish a connection to other host.
// A server side application can both work as a server host and a client host.
#ifndef __TCPClient_hpp__
#define __TCPClient_hpp__

#include <string>
#include "TCPConnection.hpp"

class TCPClient {
  public: 
    TCPConnection* connect(const std::string serverIp, int port);
};


#endif // __TCPClient_hpp__