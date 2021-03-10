#include <stdio.h>
#include "ConnectionHandler.hpp"

using namespace std;

ConnectionHandler::ConnectionHandler(WorkQueue<WorkItem*>& queue): m_queue(queue) {};

void* ConnectionHandler::run() {
  int i;
  
  while (true) {
    printf("[thread=%lu][%d] waiting for item...\n", getId(), i);
    WorkItem* item = m_queue.pop();
    printf("[thread=%lu][%d] got an item...\n", getId(), i);

    TCPConnection* connection = item->getConnection();

    char msg[256];
    int length;
    while((length = connection->receive(msg, sizeof(msg)-1)) > 0) {
      msg[length] = '\0';
      connection->send(msg, length);
      printf("[thread=%lu][%d] echoed '%s' back\n", getId(), i, msg);
    }

    delete item;

    i++;
  }

  return NULL;
}