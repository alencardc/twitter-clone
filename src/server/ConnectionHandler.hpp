#ifndef __CONNECTION_HANDLER_HPP__
#define __CONNECTION_HANDLER_HPP__

#include <string>
#include <utility>
#include "lib/Thread.hpp"
#include "lib/socket/TCPConnection.hpp"
#include "server/session/SessionManager.hpp"

class ConnectionHandler : public Thread {
  TCPConnection* m_connection;
  SessionManager& m_sessionManager;

  public: 
    ConnectionHandler(TCPConnection* connection, SessionManager& manager);

  private: 
    void* run();

    std::pair<std::string, long unsigned int> authorizeSession();

};


#endif // __CONNECTION_HANDLER_HPP__