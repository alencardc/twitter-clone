#include <stdio.h>
#include <stdlib.h>
#include <algorithm>

#include "GlobalConfig.hpp"
#include "lib/SyncAccess.hpp"
#include "lib/socket/TCPServer.hpp"
#include "lib/socket/TCPClient.hpp"
#include "frontend/ClientHandler.hpp"

TCPServer* startServerInAnyAddress();
void restablishConnections(std::vector<ClientHandler*>& clientHandlers);
bool clientConnectionIsClosed(ClientHandler* handler);


int main() {

  TCPServer* server = startServerInAnyAddress();

  if (server == NULL) {
    printf("[Front-end] Unable to start server. All addresses are being used.\n");
    return -1;
  }

  printf("[Front-end] Server started. Listening at: %s:%d\n", server->ip().c_str(), server->port());


  std::vector<ClientHandler*> clientHandlers;
  SyncAccess<bool> shouldRestablish;
  shouldRestablish.set(false);
  while (true) {
    if (shouldRestablish.get() == true) {
      printf("[Front-end] Restablishing connections...\n");
      restablishConnections(clientHandlers);
      shouldRestablish.set(false);
    }

    TCPConnection* clientConn = server->accept(800);

    if (clientConn != NULL) {
      printf("[Front-end] New connection established\n");
      TCPClient client;

      TCPConnection* serverConn = client.connect(
        GlobalConfig::SERVER_ADDR.ip, GlobalConfig::SERVER_ADDR.port
      );

      ClientHandler* handler = new ClientHandler(
        clientConn, serverConn, shouldRestablish
      );
      handler->start();
      clientHandlers.push_back(handler);
    }
  }


  delete server; server = NULL;
  return 0;
}


TCPServer* startServerInAnyAddress() {
  srand(42);
  TCPServer* server = NULL;
  unsigned tries = 0;
  unsigned idxCurr = rand() % GlobalConfig::FRONTEND_ADDRESSES.size();

  bool started = false;
  while (started == false && tries < GlobalConfig::FRONTEND_ADDRESSES.size()) {
    unsigned idxAddr = idxCurr % GlobalConfig::FRONTEND_ADDRESSES.size();

    auto addr = GlobalConfig::FRONTEND_ADDRESSES[idxAddr];
    server = new TCPServer(addr.ip, addr.port);
    started = server->start();

    if (started == false) {
      delete server;
      server = NULL;
    }
    tries += 1;
    idxCurr += 1;
  }

  return server;
}

bool clientConnectionIsClosed(ClientHandler* handler) {
  return handler->isFinished;
}

void restablishConnections(std::vector<ClientHandler*>& clientHandlers) {
  // Remove closed connection handlers
  clientHandlers.erase(
    std::remove_if(clientHandlers.begin(), clientHandlers.end(), clientConnectionIsClosed),
    clientHandlers.end()
  );
  printf("Restablishing...\n");
  // Restablish connections
  for (ClientHandler* handler : clientHandlers) {
    printf("Restablished\n");
    TCPClient client;
    TCPConnection* serverConn = client.connect(
      GlobalConfig::SERVER_ADDR.ip, GlobalConfig::SERVER_ADDR.port
    );
    if (serverConn != NULL) {
      Packet reconnectionPacket = Packet(LOGIN, handler->username.c_str());
      serverConn->send(&reconnectionPacket);
    }
    handler->setServerConnection(serverConn);
    handler->restartServerConsumer();
  }
}