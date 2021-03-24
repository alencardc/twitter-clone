#include "lib/packet/Packet.hpp"
#include "NotificationConsumer.hpp"

NotificationConsumer::NotificationConsumer(
  TCPConnection* connection,
  std::string username,
  NotificationManager& notificationManager
):
  m_connection(connection),
  m_username(username),
  m_notificationManager(notificationManager),
  m_shouldStop(false)
{}

void* NotificationConsumer::run() {
  printf("[thread=%lu][Consumer][%s] started...\n", getId(), m_username.c_str());

  m_notificationManager.subscribe(m_username, getId(), m_queue);

  printf("[thread=%lu][Consumer][%s] subscribed...\n", getId(), m_username.c_str());

  while(shouldStop() == false) {
    //printf("[thread=%lu][Consumer][%s] waiting...\n", getId(), m_username.c_str());

    auto pendingOrError = m_queue.tryRemoveFor(1500);

    if (pendingOrError.first == true) {
      PendingNotification pending = pendingOrError.second;
      printf("[thread=%lu][Consumer][%s] removed: <@%s, %d>\n",
        getId(),
        m_username.c_str(),
        pending.sender.c_str(),
        pending.notificationId
      );

      auto notificationOrError = m_notificationManager.readNotification(pending);
      if (notificationOrError.first == true) {
        Packet packet = Packet(DATA, &notificationOrError.second);
        m_connection->send(&packet);

        printf("[thread=%lu][Consumer][%s] sent: <@%s, %d>\n", 
          getId(),
          m_username.c_str(),
          pending.sender.c_str(),
          pending.notificationId
        );
      } else {
        printf("[thread=%lu][Consumer][%s] not found: <@%s, %d>\n", 
          getId(),
          m_username.c_str(),
          pending.sender.c_str(),
          pending.notificationId
        );
      }
    }

  }

  m_notificationManager.unsubscribe(m_username, getId());
  printf("[thread=%lu][Consumer][%s] unsubscribed...\n", getId(), m_username.c_str());
  printf("[thread=%lu][Consumer][%s] finished...\n", getId(), m_username.c_str());
  return NULL;
}

void NotificationConsumer::stop() {
  m_mutex.lock();
  m_shouldStop = true;
  m_mutex.unlock();
}

bool NotificationConsumer::shouldStop() {
  m_mutex.lock();
  bool stop = m_shouldStop;
  m_mutex.unlock();
  return stop;
}