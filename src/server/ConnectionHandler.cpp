#include <stdio.h>
#include <iostream>
#include <string>
#include "ConnectionHandler.hpp"
#include "lib/utils/string.hpp"

using namespace std;

ConnectionHandler::ConnectionHandler(
  TCPConnection* connection,
  SessionManager& manager
): 
  m_connection(connection),
  m_sessionManager(manager)
{};

void* ConnectionHandler::run() {
  printf("[thread=%lu] started...\n", getId());

  char msg[256];
  int length;

  bool authorized = authorizeSession();

  if (authorized == true) {
    m_connection->send("ok", 3);
    printf("[thread=%lu] session started\n", getId());
    while((length = m_connection->receive(msg, sizeof(msg)-1)) > 0) {
      msg[length] = '\0';
      m_connection->send(msg, length);
      printf("[thread=%lu] echoed '%s' back\n", getId(), msg);
    }
  }

  printf("[thread=%lu] finished...\n", getId());
  delete m_connection;

  return NULL;
}

bool ConnectionHandler::authorizeSession() {
  Packet* packet = m_connection->receive();
  
  if (packet != NULL && packet->length() > 0) {
    std::string payload = packet->payload();
    if (hasPrefix(payload, "LOGIN ") == true) {
      std::string username = removePrefix(payload, "LOGIN ");
      
      printf("[thread=%lu] attempt to start session as: %s\n", getId(), username.c_str());
      bool isStarted = m_sessionManager.startSession(username, 1);
      return isStarted;
    }
  }
  printf("[thread=%lu] bad request to start session\n", getId());
  return false;
}