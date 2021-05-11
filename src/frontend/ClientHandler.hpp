#ifndef __ClientHandler_hpp__
#define __ClientHandler_hpp__

#include <mutex>

#include "lib/Thread.hpp"
#include "lib/SyncAccess.hpp"
#include "lib/socket/TCPConnection.hpp"
#include "frontend/ServerConsumer.hpp"

class ClientHandler : public Thread {
  TCPConnection* m_clientConn;
  TCPConnection* m_serverConn;
  SyncAccess<bool>& m_shouldRestablish;

  std::mutex m_mutex;

  ServerConsumer* m_serverConsumer;

  public:
    std::string username;
    bool isFinished;

    ClientHandler(
      TCPConnection* clientConn,
      TCPConnection* serverConn,
      SyncAccess<bool>& shouldRestablish
    );

    void setServerConnection(TCPConnection* conn);
    void restartServerConsumer();

  private:
    void* run();

};

#endif // __ClientHandler_hpp__