#include <stdio.h>
#include "lib/utils/string.hpp"
#include "ConnectionHandler.hpp"
#include "routes/FollowRoute.hpp"
#include "routes/SendTweetRoute.hpp"

using namespace std;

ConnectionHandler::ConnectionHandler(
  TCPConnection* connection,
  ProfileManager& profileManager
): 
  m_connection(connection),
  m_profileManager(profileManager)
{};

void* ConnectionHandler::run() {
  printf("[thread=%lu] started...\n", getId());

  std::pair<std::string, long unsigned int> sessionToken;
  sessionToken = authorizeSession();


  if (sessionToken.second == getId()) {
    Packet* response = new Packet(OK, "");
    m_connection->send(response);
    delete response;
    printf("[thread=%lu] session started\n", getId());

    Packet* request;
    while((request = m_connection->receive()) != NULL) {
      printf("[thread=%lu] received a %s request\n", getId(), request->typeString().c_str());
      if (request->type() == SEND) {
        SendTweetRoute route = SendTweetRoute(*request);
        response = route.execute();
      } else if (request->type() == FOLLOW) {
        FollowRoute route = FollowRoute(sessionToken.first, *request, m_profileManager);
        response = route.execute();
      } else if (request->type() == LOGIN) {
        response = new Packet(ERROR, "Already logged in.");
      } else { // OK, ERROR, NO_TYPE or undefined
        response = new Packet(ERROR, "Bad request.");
      }

      printf("[thread=%lu] sent a %s response\n", getId(), response->typeString().c_str());
      m_connection->send(response);

      delete request;
      delete response;
    }

    m_profileManager.closeSession(sessionToken.first, sessionToken.second);
  } else {
    Packet response = Packet(ERROR, "Connection refused.");
    m_connection->send(&response);
  }

  printf("[thread=%lu] finished...\n", getId());
  delete m_connection;

  return NULL;
}

std::pair<std::string, long unsigned int> ConnectionHandler::authorizeSession() {
  Packet* packet = m_connection->receive();
  
  if (packet != NULL && packet->length() > 0 && packet->type() == LOGIN) {
    std::string username = packet->payload();
    
    printf("[thread=%lu] attempt to start session as: %s\n", getId(), username.c_str());
    bool isStarted = m_profileManager.startSession(username, Session(getId(), username));
    if (isStarted) {
      return std::make_pair(username, getId());
    }
  }
  printf("[thread=%lu] bad request to start session\n", getId());
  return std::make_pair("", 0);
}