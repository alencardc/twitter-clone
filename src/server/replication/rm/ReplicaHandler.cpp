#include <cstring>
#include <sstream>
#include <mutex> 
#include <condition_variable>
#include "ReplicaHandler.hpp"

ReplicaHandler::ReplicaHandler(
  TCPConnection* connection,
  ReplicaInfo info,
  ProfileManager& profileManager,
  NotificationManager& notificationManager,
  SyncAccess<bool>& electing,
  bool isConnectedToLeader,
  SyncAccess<bool>& lostElection,
  std::mutex& mutex,
  std::condition_variable& cv
): 
  m_connection(connection),
  m_info(info),
  m_profileManager(profileManager),
  m_notificationManager(notificationManager),
  m_isRunningElection(electing),
  m_isConnectedToLeader(isConnectedToLeader),
  m_lostElection(lostElection),
  m_mutex(mutex),
  m_cv(cv)
{};



void* ReplicaHandler::run() {
  printf("[ReplicaHandler] Started a new connection\n");
  
  Packet* request;
  while((request = m_connection->receive()) != NULL) {
    if (request->type() == ELECTION) {
      if (m_isRunningElection.get() == false)
        m_isRunningElection.set(true);
      
      std::string payload = request->payload();
      if (payload.size() > 0) {
        printf("[ReplicaHandler] Received ELECTION from <%s>\n", payload.c_str());
        ReplicaInfo replica = ReplicaInfo::deserialize(payload.c_str());

        bool isNotEqualToMe = replica.ip != m_info.ip || replica.port != m_info.port;
        bool isGreaterThanMe = replica.id < m_info.id; // Greater id has less value
        if (isNotEqualToMe && isGreaterThanMe) {
          printf("[ReplicaHandler] <%s>, your id greater than mine!\n", payload.c_str());
          Packet answerPacket = Packet(ANSWER, "Won");
          m_connection->send(&answerPacket);
        } else {
          printf("[ReplicaHandler] <%s>, your id less than mine!\n", payload.c_str());
          Packet answerPacket = Packet(ANSWER, "Lost");
          m_connection->send(&answerPacket);
        }
      }

    } else if (request->type() == COORDINATOR) {
      std::string payload = request->payload();
      if (payload.size() > 0) {
        printf("[ReplicaHandler] Received COORDINATOR from <%s>\n", payload.c_str());
        m_isConnectedToLeader = true;
      }

    } else if (request->type() == ANSWER) {
      m_mutex.lock();
      if (std::string(request->payload()) == "Lost") {
        m_lostElection.set(true);
      }
      m_mutex.unlock();
      m_cv.notify_one();
    
    } else if (request->type() == UPDATE_PROFILE) {
      printf("[ReplicaHandler] Received PROFILE update!\n");
      ProfileManager updatedManager;
      updatedManager.deserialize(request->payload());
      m_profileManager.update(updatedManager);
    
    } else if (request->type() == UPDATE_NOTIFICATION) {
      printf("[ReplicaHandler] Received NOTIFICATION update!\n");
      NotificationManager updatedManager;
      updatedManager.deserialize(request->payload());
      m_notificationManager.update(updatedManager);
    }  

    delete request; request = NULL;
  }

  // If it reaches this code, then the connection was lost!
  if (m_isConnectedToLeader)
    m_isRunningElection.set(true);

  m_connection->close(); // Ensure it's closed

  printf("[ReplicaHandler] Connection ended.\n");
  return NULL;
}