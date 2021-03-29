#include <stdio.h>
#include "ConnectionConsumer.hpp"
#include "lib/packet/Packet.hpp"
#include <ncurses.h>
#include "lib/utils/string.hpp"

ConnectionConsumer::ConnectionConsumer(
  TCPConnection& connection,
  NotificationList& feed
):
  m_connection(connection),
  m_feed(feed)
{}

void* ConnectionConsumer::run() {
  Packet* packet;

  while(true) {
    packet = m_connection.receive();
    mvwprintw(stdscr, 0,92, "Length: %d", packet->length());
    auto splited = split(packet->payload(), "\n");
    for (long unsigned i = 0; i < splited.size(); i++)
      mvwprintw(stdscr, 1+i,92, splited[i].c_str());
    if (packet == NULL) {
      //printf("Connection lost. Unable to reach the server.\n");
      break;
    } else {
      if (packet->type() == DATA) {
        Notification* notification = Notification::deserialize(packet->payload());
        m_feed.addItem(*notification);
        delete notification;
      }
      //printf("[receivedOnConsumer]: %s\n", packet->serialize().c_str());
      //m_queue.insert(packet);
    }
  }

  return NULL;
}