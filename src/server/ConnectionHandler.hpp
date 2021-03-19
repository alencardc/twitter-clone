#ifndef __CONNECTION_HANDLER_HPP__
#define __CONNECTION_HANDLER_HPP__

#include "lib/Thread.hpp"
#include "lib/socket/WorkQueue.hpp"
#include "server/WorkItem.hpp"

class ConnectionHandler : public Thread {
  TCPConnection* m_connection;
  

  public: 
    ConnectionHandler(TCPConnection* connection);

  private: 
    void* run();

};


#endif // __CONNECTION_HANDLER_HPP__