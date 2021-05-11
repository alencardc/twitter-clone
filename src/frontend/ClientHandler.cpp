#include "ClientHandler.hpp"


ClientHandler::ClientHandler(
  TCPConnection* clientConn,
  TCPConnection* serverConn,
  SyncAccess<bool>& shouldRestablish
):
  m_clientConn(clientConn),
  m_serverConn(serverConn),
  m_shouldRestablish(shouldRestablish),
  m_serverConsumer(NULL),
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

void ClientHandler::restartServerConsumer() {
  if (m_serverConsumer != NULL) {
    delete m_serverConsumer;
  }
  m_serverConsumer = new ServerConsumer(
    m_clientConn, m_serverConn, m_shouldRestablish, m_mutex
  );
  m_serverConsumer->start();
}

void* ClientHandler::run() {
  if (m_clientConn == NULL)
    return NULL;
  
  ServerConsumer* serverConsumer = new ServerConsumer(
    m_clientConn, m_serverConn, m_shouldRestablish, m_mutex
  );
  serverConsumer->start();

  printf("[Front-end][ClientHandler] Connection handler started\n");
  isFinished = false;

  Packet* request = NULL;
  while ((request = m_clientConn->receive()) != NULL) {

    if (request->type() == LOGIN) {
      username = request->payload();
    }

    printf("[Front-end][ClientHandler][%s] Received a %s packet from CLIENT\n",
      username.c_str(),
      request->typeString().c_str()
    );

    m_mutex.lock();
    if (request->type() == NEW_LEADER) {
      m_shouldRestablish.set(true);
    } else if (m_serverConn == NULL || m_shouldRestablish.get() == true) {
      Packet errorReponse = Packet(ERROR, "Connection lost. Please, retry in a few seconds.");
      m_clientConn->send(&errorReponse);
    } else {
      m_serverConn->send(request);
    }
    m_mutex.unlock();
    delete request; request = NULL;
  }
  printf("[Front-end][ClientHandler][%s] Lost client connection...\n", username.c_str());

  m_clientConn->close();
  if (m_serverConn != NULL) {
    Packet closePacket = Packet(CLOSE, "");
    m_serverConn->send(&closePacket);
    m_serverConn->close();
  }

  serverConsumer->join();
  delete serverConsumer;

  printf("[Front-end][ClientHandler][%s] Connection handler ended\n",
    username.c_str()
  );
  // Connection ended
  isFinished = true;
  return NULL;
}