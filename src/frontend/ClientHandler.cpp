#include "ClientHandler.hpp"
#include "ServerConsumer.hpp"


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
  
  ServerConsumer* serverConsumer = new ServerConsumer(
    m_clientConn, m_serverConn, m_shouldRestablish, m_mutex
  );
  serverConsumer->start();

  printf("[Front-end] Connection handler started\n");
  isFinished = false;

  Packet* request = NULL;
  while ((request = m_clientConn->receive()) != NULL) {

    if (request->type() == LOGIN) {
      username = request->payload();
    }

    printf("[Front-end] Received a %s packet from CLIENT\n", request->typeString().c_str());

    m_mutex.lock();
    if (m_serverConn == NULL || m_shouldRestablish.get() == true) {
      m_shouldRestablish.set(true);
      Packet errorReponse = Packet(ERROR, "Connection lost. Please, retry in a few seconds.");
      m_clientConn->send(&errorReponse);
      
    } else {
      m_serverConn->send(request);
    }
    m_mutex.unlock();
    delete request; request = NULL;
  }

  delete serverConsumer;

  m_clientConn->close();
  if (m_serverConn != NULL) {
    m_serverConn->close();
    m_serverConn = NULL;
  }

  printf("[Front-end] Connection handler ended\n");
  // Connection ended
  isFinished = true;
  return NULL;
}