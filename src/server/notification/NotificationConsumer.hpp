#ifndef __NotificationConsumer_hpp__
#define __NotificationConsumer_hpp__

#include <mutex>
#include "lib/Thread.hpp"
#include "lib/socket/TCPConnection.hpp"
#include "server/notification/NotificationManager.hpp"
#include "NotificationManager.hpp"

class NotificationConsumer: public Thread {
  TCPConnection* m_connection;
  std::string m_username;
  NotificationManager& m_notificationManager;
  Queue<PendingNotification> m_queue;

  bool m_shouldStop;
  std::mutex m_mutex;

  public:
    NotificationConsumer(
      TCPConnection* connection, 
      std::string username, 
      NotificationManager& 
      notificationManager
    );

    void stop();
    bool shouldStop();

  private:
    void* run();
};

#endif // __NotificationConsumer_hpp__