#include "ServerHandler.hpp"
#include "ConnectionHandler.hpp"
#include "lib/socket/TCPServer.hpp"

ServerHandler::ServerHandler(
  std::string ip, 
  int port, ProfileManager& profileManager,
  NotificationManager& notificationManager
): 
  m_ip(ip),
  m_port(port),
  m_profileManager(profileManager),
  m_notificationManager(notificationManager)
{}

void* ServerHandler::run() {
  TCPServer* serverSock = new TCPServer(m_ip, DEFAULT_SERVER_PORT);
  printf("[info] Server started! Listening at %s:%d\n", serverSock->ip().c_str(), serverSock->port());
  if (serverSock->start() == false) {
    printf("[error] Unable to start the server!\n");
    return NULL;
  }

  while (true) {
    TCPConnection* connection = serverSock->accept();
    if (connection == NULL) {
      printf("[error] Unable to accept a connection!\n");
      continue;
    }
    try {
      ConnectionHandler* handler = new ConnectionHandler(connection, m_profileManager, m_notificationManager);
      handler->start();
    } catch(std::bad_alloc& e) {
      printf("[error] Thread creation: bad_alloc caught: %s\n", e.what());
    } catch(...) {
      printf("[error] Thread creation: unknown error.\n");
    }
  }
  return NULL;
}
