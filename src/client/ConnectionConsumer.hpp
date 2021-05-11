#ifndef __ConnectionConsumer_hpp__
#define __ConnectionConsumer_hpp__

#include "lib/Thread.hpp"
#include "lib/socket/TCPConnection.hpp"
#include "lib/packet/Packet.hpp"
#include "lib/Queue.hpp"
#include "lib/SyncAccess.hpp"
#include "ui/NotificationList.hpp"
#include "ui/TimeoutLabel.hpp"

class ConnectionConsumer: public Thread {
  TCPConnection& m_connection;
  NotificationList& m_feed;
  TimeoutLabel& m_response;
  SyncAccess<bool>& m_shouldReconnect;
  public:
    ConnectionConsumer(
      TCPConnection& connection,
      NotificationList& feed,
      TimeoutLabel& label,
      SyncAccess<bool>& shouldReconnect
    );

  private:
    void* run();


};

#endif
