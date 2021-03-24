#include "NotificationQueue.hpp"

void NotificationQueue::insert(PendingNotification pending) {
  // TODO: Decrement notification
  if (m_subscribersQueue.size() > 0) {
    for (auto subscriber : m_subscribersQueue) {
      subscriber.second.insert(pending);
    }
  } else {
    m_pendingQueue.insert(pending);
  }
}

void NotificationQueue::subscribe(
  long unsigned id,
  Queue<PendingNotification>& queue
) {
  m_subscribersQueue.emplace(id, queue);
}

void NotificationQueue::unsubscribe(long unsigned id) {
  if (m_subscribersQueue.count(id) == 1) {
    m_subscribersQueue.erase(id);
  }
}