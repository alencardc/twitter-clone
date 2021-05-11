#include <stdio.h>
#include "ConnectionConsumer.hpp"
#include "lib/packet/Packet.hpp"
#include <ncurses.h>
#include "lib/utils/string.hpp"

ConnectionConsumer::ConnectionConsumer(
  TCPConnection& connection,
  NotificationList& feed,
  TimeoutLabel& label,
  SyncAccess<bool>& shouldReconnect
):
  m_connection(connection),
  m_feed(feed),
  m_response(label),
  m_shouldReconnect(shouldReconnect)
{}

void* ConnectionConsumer::run() {
  Packet* packet;

  while(m_shouldReconnect.get() == false) {
    packet = m_connection.receive();
    if (packet == NULL) {
      m_response.setText("Connection lost. Trying to reconnect...");
      m_shouldReconnect.set(true);
    } else {
      if (packet->type() == DATA) {
        Notification* notification = Notification::deserialize(packet->payload());
        m_feed.addItem(*notification);
        delete notification;
      } else if (packet->type() == SUCCESS || packet->type() == ERROR) {
        m_response.setText(packet->payload());
      }
    }
  }

  return NULL;
}