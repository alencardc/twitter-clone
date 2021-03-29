#ifndef __ConnectionConsumer_hpp__
#define __ConnectionConsumer_hpp__

#include "lib/Thread.hpp"
#include "lib/socket/TCPConnection.hpp"
#include "lib/packet/Packet.hpp"
#include "lib/Queue.hpp"
#include "ui/NotificationList.hpp"
#include "ui/TimeoutLabel.hpp"

class ConnectionConsumer: public Thread {
  TCPConnection& m_connection;
  NotificationList& m_feed;
  TimeoutLabel& m_response;
  public:
    ConnectionConsumer(TCPConnection& connection, NotificationList& feed, TimeoutLabel& label);

  private:
    void* run();
};

#endif
