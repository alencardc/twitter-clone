#include <stdio.h>
#include "lib/utils/string.hpp"
#include "ConnectionHandler.hpp"
#include "routes/FollowRoute.hpp"
#include "routes/SendTweetRoute.hpp"
#include "notification/NotificationConsumer.hpp"

using namespace std;

ConnectionHandler::ConnectionHandler(
  TCPConnection* connection,
  ProfileManager& profileManager,
  NotificationManager& notificationManager
): 
  m_connection(connection),
  m_profileManager(profileManager),
  m_notificationManager(notificationManager)
{};

void* ConnectionHandler::run() {
  printf("[thread=%lu][Producer][] started...\n", getId());

  std::pair<std::string, long unsigned int> sessionToken;
  sessionToken = authorizeSession();


  if (sessionToken.second == getId()) {
    NotificationConsumer* consumerThread = new NotificationConsumer(
      m_connection, 
      sessionToken.first, 
      m_notificationManager
    );
    consumerThread->start();


    Packet* response = new Packet(SUCCESS, "");
    m_connection->send(response);
    delete response;
    printf("[thread=%lu][Producer][%s] session started\n",
      getId(),
      sessionToken.first.c_str()
    );

    Packet* request;
    while((request = m_connection->receive()) != NULL) {
      printf("[thread=%lu][Producer][%s] received a %s request\n",
        getId(),
        sessionToken.first.c_str(),
        request->typeString().c_str()
      );
      if (request->type() == SEND) {
        SendTweetRoute route = SendTweetRoute(
          sessionToken.first, 
          *request,
          m_notificationManager,
          m_profileManager
        );
        response = route.execute();
      } else if (request->type() == FOLLOW) {
        FollowRoute route = FollowRoute(sessionToken.first, *request, m_profileManager);
        response = route.execute();
      } else if (request->type() == LOGIN) {
        response = new Packet(ERROR, "Already logged in.");
      } else { // SUCCESS, ERROR, NO_TYPE or undefined
        response = new Packet(ERROR, "Bad request.");
      }

      printf("[thread=%lu][Producer][%s] sent a %s response\n", 
        getId(), 
        sessionToken.first.c_str(),
        response->typeString().c_str()
      );
      m_connection->send(response);

      delete request;
      delete response;
    }

    consumerThread->stop();
    consumerThread->join();
    delete consumerThread;

    m_profileManager.closeSession(sessionToken.first, sessionToken.second);
  } else {
    Packet response = Packet(ERROR, "Connection refused.");
    m_connection->send(&response);
  }

  printf("[thread=%lu][Producer][%s] finished...\n", getId(), sessionToken.first.c_str());
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