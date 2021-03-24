#include "NotificationQueue.hpp"

// Returns true if Notification was delivered to subscribers
// Return false if there is no subscribers available (no session/user offline)
// so it means the notification is pending to be sent
bool NotificationQueue::insert(Notification notification) {
  if (m_subscribersQueue.size() > 0) {
    for (auto subscriber : m_subscribersQueue) {
      subscriber.second.insert(notification);
    }
    return true;
  }

  PendingNotification pending(notification.username, notification.id);
  m_pendingQueue.insert(pending);

  return false;
}

bool NotificationQueue::hasPending() {
  return m_pendingQueue.size() != 0;
}

PendingNotification NotificationQueue::removePending() {
  if (m_pendingQueue.size() != 0) {
    return m_pendingQueue.remove();
  }

  return PendingNotification();
}

bool NotificationQueue::insertToSubscribers(Notification notification) {
  if (m_subscribersQueue.size() > 0) {
    for (auto subscriber : m_subscribersQueue) {
      subscriber.second.insert(notification);
    }
    return true;
  }

  return false;
}

void NotificationQueue::subscribe(
  long unsigned id,
  Queue<Notification>& queue
) {
  m_subscribersQueue.emplace(id, queue);
  // if (m_subscribersQueue.size() == 1 && m_pendingQueue.size() > 0) { 
  //   PendingNotification pending;
  //   while (m_pendingQueue.size() != 0) {
  //     pending = m_pendingQueue.remove();
  //     //queue.insert(pending);
  //   }
  // }
}

void NotificationQueue::unsubscribe(long unsigned id) {
  if (m_subscribersQueue.count(id) == 1) {
    m_subscribersQueue.erase(id);
  }
}