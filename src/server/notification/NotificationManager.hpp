#ifndef __NotificationManager_hpp__
#define __NotificationManager_hpp__

#include <mutex>
#include <list>
#include <unordered_map>
#include "server/notification/Notification.hpp"
#include "server/notification/NotificationQueue.hpp"

class NotificationManager {
  std::unordered_map<std::string, std::list<Notification>> m_notifications;
  std::unordered_map<std::string, NotificationQueue> m_pendingQueues;

  std::mutex m_mutex;

  public:
    void send(Notification notification, std::list<std::string> followers);

    std::pair<bool, Notification> readNotification(PendingNotification pending);

    void subscribe(
      std::string username,
      long unsigned id,
      Queue<PendingNotification>& queue
    );

    void unsubscribe(std::string username, long unsigned id);

  private:
    bool createNotification(Notification notification);

};

#endif // __NotificationManager_hpp__