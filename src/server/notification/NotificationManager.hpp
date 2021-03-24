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

  unsigned int m_nextId;

  public:
    NotificationManager();

    void send(Notification notification, std::list<std::string> followers);

    void subscribe(
      std::string username,
      long unsigned id,
      Queue<Notification>& queue
    );

    void unsubscribe(std::string username, long unsigned id);

  private:
    unsigned int getNextId();
    std::pair<bool, Notification> findNotification(PendingNotification pending);
    bool createNotification(Notification& notification);

};

#endif // __NotificationManager_hpp__