#include "ServerConsumer.hpp"


ServerConsumer::ServerConsumer(
  TCPConnection* clientConn,
  TCPConnection*& serverConn,
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

  printf("[Front-end][ServerConsumer] Started\n");

  Packet* request = NULL;
  while (m_serverConn != NULL && m_shouldRestablish.get() == false) {

    request = m_serverConn->receive();
    if (request == NULL) // Connection lost
      break;

    printf("[Front-end][ServerConsumer]Received a %s packet from SERVER\n",
      request->typeString().c_str()
    );
    m_clientConn->send(request);
    
    delete request; request = NULL;
  }

  printf("[Front-end][ServerConsumer] Lost server connection...\n");
  m_mutex.lock();
  if (m_serverConn != NULL) {
    m_serverConn->close();
    delete m_serverConn;
    m_serverConn = NULL;
  }
  // if (m_clientConn->isClosed() == false)
  //   m_shouldRestablish.set(true); // Connection ended
  m_mutex.unlock();

  printf("[Front-end][ServerConsumer] Finished\n");

  return NULL;
}