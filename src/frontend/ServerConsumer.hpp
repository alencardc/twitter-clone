#ifndef __ServerConsumer_hpp__
#define __ServerConsumer_hpp__

#include <mutex>

#include "lib/Thread.hpp"
#include "lib/SyncAccess.hpp"
#include "lib/socket/TCPConnection.hpp"

class ServerConsumer : public Thread {
  TCPConnection* m_clientConn;
  TCPConnection*& m_serverConn;
  SyncAccess<bool>& m_shouldRestablish;
  std::mutex& m_mutex;

  public:

    ServerConsumer(
      TCPConnection* clientConn,
      TCPConnection*& serverConn,
      SyncAccess<bool>& shouldRestablish,
      std::mutex& mutex
    );

  private:
    void* run();

};

#endif // __ServerConsumer_hpp__