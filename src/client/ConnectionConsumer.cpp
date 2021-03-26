#include <stdio.h>
#include "ConnectionConsumer.hpp"

ConnectionConsumer::ConnectionConsumer(
  TCPConnection& connection,
  Queue<Packet*>& queue
):
  m_connection(connection),
  m_queue(queue)
{}

void* ConnectionConsumer::run() {
  printf("Consumer running\n");
  Packet* packet;
  int i = 0;

  while(true) {
    packet = m_connection.receive();
    printf("\nIt: %d\n", i);
    i+=1;
    if (packet == NULL) {
      printf("Connection lost. Unable to reach the server.\n");
      break;
    } else {
      printf("[receivedOnConsumer]: %s\n", packet->serialize().c_str());
      m_queue.insert(packet);
    }
  }

  return NULL;
}