#ifndef __ConnectionConsumer_hpp__
#define __ConnectionConsumer_hpp__

#include "lib/Thread.hpp"
#include "lib/socket/TCPConnection.hpp"
#include "lib/packet/Packet.hpp"
#include "lib/Queue.hpp"

class ConnectionConsumer: public Thread {
  TCPConnection& m_connection;
  Queue<Packet*>& m_queue;
  public:
    ConnectionConsumer(TCPConnection& connection, Queue<Packet*>& queue);

  private:
    void* run();
};

#endif
