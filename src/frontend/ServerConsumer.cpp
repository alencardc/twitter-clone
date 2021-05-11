#include "ServerConsumer.hpp"


ServerConsumer::ServerConsumer(
  TCPConnection* clientConn,
  TCPConnection* serverConn,
  SyncAccess<bool>& shouldRestablish,
  std::mutex& mutex
):
  m_clientConn(clientConn),
  m_serverConn(serverConn),
  m_shouldRestablish(shouldRestablish),
  m_mutex(mutex)
{}

void* ServerConsumer::run() {
  if (m_clientConn == NULL)
    return NULL;

  Packet* request = NULL;
  while (m_serverConn != NULL && m_shouldRestablish.get() == false) {

    request = m_serverConn->receive();
    if (request == NULL) // Connection lost
      break;

    printf("[Front-end] Received a %s packet from SERVER\n", request->typeString().c_str());
    m_clientConn->send(request);
    
    delete request; request = NULL;
  }

  m_mutex.lock();
  m_serverConn->close();
  m_serverConn = NULL;
  m_shouldRestablish.set(true); // Connection ended
  m_mutex.unlock();

  return NULL;
}