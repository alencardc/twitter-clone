#ifndef __ConnectionConsumer_hpp__
#define __ConnectionConsumer_hpp__

#include "lib/Thread.hpp"
#include "lib/socket/TCPConnection.hpp"
#include "lib/packet/Packet.hpp"
#include "lib/Queue.hpp"
#include "ui/NotificationList.hpp"

class ConnectionConsumer: public Thread {
  TCPConnection& m_connection;
  NotificationList& m_feed;
  public:
    ConnectionConsumer(TCPConnection& connection, NotificationList& feed);

  private:
    void* run();
};

#endif
