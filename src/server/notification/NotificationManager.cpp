#include <algorithm>
#include "NotificationManager.hpp"

void NotificationManager::send(
  Notification notification, 
  std::list<std::string> followers
) {
  m_mutex.lock();
  if (createNotification(notification) == true) {
    for (std::string follower : followers) {
      if (m_pendingQueues.count(follower) == 1) {
        PendingNotification pending(notification.username, notification.id);
        m_pendingQueues[follower].insert(pending);
      }
    }
  }
  m_mutex.unlock();
}

std::pair<bool, Notification> NotificationManager::readNotification(
  PendingNotification pending
) {
  m_mutex.lock();
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
  m_mutex.unlock();
  return std::make_pair(success, result);
}

void NotificationManager::subscribe(
  std::string username,
  long unsigned id,
  Queue<PendingNotification>& queue
) {
  m_mutex.lock();

  if (m_pendingQueues.count(username) == 1) {
    m_pendingQueues[username].subscribe(id, queue);
  } else { // CHECAR SE ESSE LUGAR ESTA CORRETO PARA ADICAO DE NOVOS USUARIOS
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


bool NotificationManager::createNotification(Notification notification) {
  bool success = false;
  if (m_notifications.count(notification.username) == 1) {
    m_notifications[notification.username].push_back(notification);
    success = true;
  }
  return success;
}