#include <stdio.h>
#include "ConnectionHandler.hpp"

using namespace std;

ConnectionHandler::ConnectionHandler(TCPConnection* connection): 
  m_connection(connection) 
{};

void* ConnectionHandler::run() {
  printf("[thread=%lu] started...\n", getId());

  char msg[256];
  int length;
  while((length = m_connection->receive(msg, sizeof(msg)-1)) > 0) {
    msg[length] = '\0';
    m_connection->send(msg, length);
    printf("[thread=%lu] echoed '%s' back\n", getId(), msg);
  }

  printf("[thread=%lu] finished...\n", getId());
  delete m_connection;

  return NULL;
}