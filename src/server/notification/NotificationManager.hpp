#ifndef __NotificationManager_hpp__
#define __NotificationManager_hpp__

#include <mutex>
#include <list>
#include <vector>
#include <functional>
#include <unordered_map>
#include "server/notification/Notification.hpp"
#include "server/notification/NotificationQueue.hpp"

class NotificationManager {
  std::unordered_map<std::string, std::list<Notification>> m_notifications;
  std::unordered_map<std::string, NotificationQueue> m_pendingQueues;

  std::mutex m_mutex;

  unsigned int m_nextId;

  public:
    std::function<bool(NotificationManager&)> onUpdateCallback;
    NotificationManager();

    void send(Notification notification, std::list<std::string> followers);

    void subscribe(
      std::string username,
      long unsigned id,
      Queue<Notification>& queue
    );

    void unsubscribe(std::string username, long unsigned id);

    void loadUsers(std::vector<std::string> usernames);

    void update(NotificationManager& manager);
    std::string serialize();
    void deserialize(std::string raw);

  private:
    unsigned int getNextId();
    std::pair<bool, Notification> findNotification(PendingNotification pending);
    bool createNotification(Notification& notification);


};

#endif // __NotificationManager_hpp__