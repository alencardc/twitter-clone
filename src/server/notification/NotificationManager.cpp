#include <algorithm>
#include "NotificationManager.hpp"

NotificationManager::NotificationManager(): m_nextId(1) {}

unsigned int NotificationManager::getNextId() {
  unsigned currentId = m_nextId;
  m_nextId += 1;
  return currentId;
}

void NotificationManager::send(
  Notification notification, 
  std::list<std::string> followers
) {
  m_mutex.lock();
  
  if (createNotification(notification) == true) {
    // Reference to the notification inserted in the list for username
    Notification& notificationRef = m_notifications[notification.username].back();
    for (std::string follower : followers) {
      if (m_pendingQueues.count(follower) == 1) {
        bool wasSent = m_pendingQueues[follower].insert(notification);
        if (wasSent == true) {
          notificationRef.pendingCount -= 1;
        }
      }
    }

    if (notificationRef.pendingCount <= 0) {
      // Notification was sent to every follower, so it can be removed from list.
      m_notifications[notification.username].pop_back(); 
    }
  }
  m_mutex.unlock();
}

std::pair<bool, Notification> NotificationManager::findNotification(
  PendingNotification pending
) {
  bool success = false;
  Notification result = Notification();

  if (m_notifications.count(pending.sender) == 1) {
    int idToSearch = pending.notificationId;
    auto notification = std::find_if(
      m_notifications[pending.sender].begin(),
      m_notifications[pending.sender].end(),
      [idToSearch](const Notification& n) -> bool { return n.id == idToSearch;}
    );

    if (notification != m_notifications[pending.sender].end()) {
      result = *notification;
      success = true;
      (*notification).pendingCount -= 1;
      if ((*notification).pendingCount <= 0) {
        m_notifications[pending.sender].erase(notification);
      }
    }
  }
  return std::make_pair(success, result);
}

void NotificationManager::subscribe(
  std::string username,
  long unsigned id,
  Queue<Notification>& queue
) {
  m_mutex.lock();

  if (m_pendingQueues.count(username) == 1) {
    m_pendingQueues[username].subscribe(id, queue);

    while (m_pendingQueues[username].hasPending() == true) {
      PendingNotification pending = m_pendingQueues[username].removePending();
      if (m_notifications.count(pending.sender) == 1) {
        auto notificationOrError = findNotification(pending);
        if (notificationOrError.first == true) {
          m_pendingQueues[username].insertToSubscribers(notificationOrError.second);
        }
      }
    }
  } else {
    std::list<Notification> list = std::list<Notification>();
    NotificationQueue notificationQueue = NotificationQueue();
    m_notifications.emplace(username, list);
    m_pendingQueues.emplace(username, notificationQueue);
    m_pendingQueues[username].subscribe(id, queue);
  }

  m_mutex.unlock();
}

void NotificationManager::unsubscribe(std::string username, long unsigned id) {
  m_mutex.lock();
  if (m_pendingQueues.count(username) == 1) {
    m_pendingQueues[username].unsubscribe(id);
  }
  m_mutex.unlock();
}


bool NotificationManager::createNotification(Notification& notification) {
  if (m_notifications.count(notification.username) == 1) {
    notification.id = getNextId();
    m_notifications[notification.username].push_back(notification);
    return true;
  }
  return false;
}