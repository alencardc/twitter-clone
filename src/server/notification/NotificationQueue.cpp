#include <deque>
#include <sstream>
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
}

void NotificationQueue::unsubscribe(long unsigned id) {
  if (m_subscribersQueue.count(id) == 1) {
    m_subscribersQueue.erase(id);
  }
}

std::string NotificationQueue::serialize() {
  std::deque<PendingNotification> pendingQueue = m_pendingQueue.get();
  std::string s = std::to_string(pendingQueue.size());
  for (PendingNotification pending : pendingQueue) {
    std::string strPending = pending.serialize();
    s.append(" " + std::to_string(strPending.size()) + " " + strPending);
  }
  return s;
}

void NotificationQueue::deserialize(std::string raw) {
  std::istringstream stream(raw);
  int size;
  stream >> size;
  
  std::deque<PendingNotification> pendingQueue;
  for (int i = 0; i < size; i++) {
    int length;
    stream >> length;
    stream.ignore(1);
    char buff[length];
    stream.read(buff, length);
    buff[length] = '\0';
    PendingNotification pending;
    pending.deserialize(buff);
    pendingQueue.push_back(pending);
  }

  m_pendingQueue.set(pendingQueue);
}