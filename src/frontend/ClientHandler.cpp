#include "ClientHandler.hpp"


ClientHandler::ClientHandler(
  TCPConnection* clientConn,
  TCPConnection* serverConn,
  SyncAccess<bool>& shouldRestablish
):
  m_clientConn(clientConn),
  m_serverConn(serverConn),
  m_shouldRestablish(shouldRestablish),
  isFinished(true)
{}

void ClientHandler::setServerConnection(TCPConnection* conn) {
  m_mutex.lock();
  if (m_serverConn != NULL) {
    m_serverConn->close();
    delete m_serverConn; 
    m_serverConn = NULL;
  }
  m_serverConn = conn;
  m_mutex.unlock();
}

void* ClientHandler::run() {
  if (m_clientConn == NULL)
    return NULL;
  
  printf("[Front-end] Connection handler started\n");
  isFinished = false;

  Packet* request = NULL;
  while ((request = m_clientConn->receive()) != NULL) {
    m_mutex.lock();

    if (request->type() == LOGIN) {
      username = request->payload();
    }

    if (m_serverConn == NULL || m_shouldRestablish.get() == true) {
      if (request->type() == NEW_LEADER) {
        m_shouldRestablish.set(true);
      } else {
        Packet errorReponse = Packet(ERROR, "Connection lost. Please, retry in a few seconds.");
        m_clientConn->send(&errorReponse);
      }
    } else {
      m_serverConn->send(request);

      Packet* response = m_serverConn->receive();

      if (response == NULL) { // Connection lost
        m_serverConn = NULL;
      } else {
        m_clientConn->send(response);
        delete response; response = NULL;
      }
    }
    
    m_mutex.unlock();
    delete request; request = NULL;
  } 

  printf("[Front-end] Connection handler ended\n");
  // Connection ended
  isFinished = true;
  return NULL;
}