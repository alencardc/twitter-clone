#ifndef __NotificationQueue_hpp__
#define __NotificationQueue_hpp__

#include <unordered_map>
#include "lib/Queue.hpp"
#include "Notification.hpp"
#include "PendingNotification.hpp"

class NotificationQueue {
  std::unordered_map<unsigned int, Queue<Notification>&> m_subscribersQueue; 
  Queue<PendingNotification> m_pendingQueue;

  public:
    NotificationQueue() = default;
    NotificationQueue(const NotificationQueue&) {} // TODO: Check this constructors
    ~NotificationQueue() = default;

    // Returns true if Notification was delivered to subscribers
    // Return false if there is no subscribers available (no session/user offline)
    // so it means the notification is pending to be sent
    bool insert(Notification notification);

    bool hasPending();
    PendingNotification removePending();
    bool insertToSubscribers(Notification notification);
    

    void subscribe(long unsigned id, Queue<Notification>& queue);

    void unsubscribe(long unsigned id);

};

#endif // __NotificationQueue_hpp__