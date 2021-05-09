#include <sstream>
#include <algorithm>
#include "lib/utils/string.hpp"
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
    onUpdateCallback(*this);
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

void NotificationManager::loadUsers(std::vector<std::string> usernames) {
  m_mutex.lock();
  for (std::string username : usernames) {
    std::list<Notification> notifications;
    NotificationQueue queue;
    m_notifications.emplace(username, notifications);
    m_pendingQueues.emplace(username, queue);
  }
  m_mutex.unlock();
}

void NotificationManager::update(NotificationManager& manager) {
  m_nextId = manager.m_nextId;
  m_pendingQueues = manager.m_pendingQueues;
  m_notifications = manager.m_notifications;
}

std::string NotificationManager::serialize() {
  std::string s = std::to_string(m_nextId) + "\n";
  for (auto userAndNotification : m_notifications) {
    bool first = true;
    s.append(userAndNotification.first + " " + std::to_string(userAndNotification.second.size()) + " ");
    for (Notification n : userAndNotification.second) {
      std::string rawNotification = n.serializeAll();
      if (first) {
        s.append(std::to_string(rawNotification.size()) + " " + rawNotification);
        first = false;
      } else 
        s.append("," + std::to_string(rawNotification.size()) + " " + rawNotification);
    }
    s.append("\n");
  }

  s.append("\n&\n");
  for (auto queue : m_pendingQueues) {
    std::string serialized = m_pendingQueues[queue.first].serialize();
    s.append(queue.first + " " + std::to_string(serialized.size()) + " " + serialized + "\n");
  }

  return s;
}

void NotificationManager::deserialize(std::string raw) {
  std::unordered_map<std::string, std::list<Notification>> notificationsMap;

  std::vector<std::string> idAndRest = splitFirst(raw, "\n");

  if (idAndRest.size() != 2)
    return;

  m_nextId = std::stoi(idAndRest[0]);

  std::vector<std::string> notificationsAndQueues = splitFirst(idAndRest[1], "\n&\n");

  if (notificationsAndQueues.size() != 2)
    return;

  std::vector<std::string> rawNotifications = split(notificationsAndQueues[0], "\n");
  for (std::string strNotification : rawNotifications) {
    if (strNotification.empty())
      continue;
    
    std::list<Notification> list;
    std::istringstream stream(strNotification);
    std::string key;
    stream >> key;
    int size;
    stream >> size;
    for (int i = 0; i < size; i++) {
      int length;
      stream >> length;
      stream.ignore(1);
      char notificationBuffer[length];
      stream.read(notificationBuffer, length);
      notificationBuffer[length] = '\0';
      Notification notification;
      notification.deserializeAll(notificationBuffer);

      list.push_back(notification);
      stream.ignore(1);
    }
    notificationsMap.emplace(key, list);
  }
  m_notifications.clear();
  m_notifications = notificationsMap;

  std::unordered_map<std::string, NotificationQueue> queuesMap;
  std::vector<std::string> rawQueues = split(notificationsAndQueues[1], "\n");
  for (std::string strQueue : rawQueues) {
    if (strQueue.empty())
      continue;
      
    std::istringstream stream(strQueue);
    std::string key;
    stream >> key;
    int length;
    stream >> length;
    stream.ignore(1);
    char queueBuffer[length];
    stream.read(queueBuffer, length);
    queueBuffer[length] = '\0';
    NotificationQueue queue;
    queue.deserialize(queueBuffer);

    queuesMap.emplace(key, queue);
  }

  m_pendingQueues.clear();
  m_pendingQueues = queuesMap;
}