#ifndef __NotificationQueue_hpp__
#define __NotificationQueue_hpp__

#include <unordered_map>
#include "lib/Queue.hpp"
#include "PendingNotification.hpp"

class NotificationQueue {
  std::unordered_map<unsigned int, Queue<PendingNotification>&> m_subscribersQueue; 
  Queue<PendingNotification> m_pendingQueue;

  public:
    NotificationQueue() = default;
    NotificationQueue(const NotificationQueue&) {} // TODO: Check this constructors
    ~NotificationQueue() = default;

    void insert(PendingNotification pending);

    void subscribe(long unsigned id, Queue<PendingNotification>& queue);

    void unsubscribe(long unsigned id);

};

#endif // __NotificationQueue_hpp__