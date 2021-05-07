#include <cstring>
#include <sstream>
#include <mutex> 
#include <condition_variable>
#include "ReplicaHandler.hpp"

ReplicaHandler::ReplicaHandler(
  TCPConnection* connection,
  ReplicaInfo info,
  SyncAccess<bool>& electing,
  bool isConnectedToLeader,
  SyncAccess<bool>& lostElection,
  std::mutex& mutex,
  std::condition_variable& cv
): 
  m_connection(connection),
  m_info(info),
  m_isRunningElection(electing),
  m_isConnectedToLeader(isConnectedToLeader),
  m_lostElection(lostElection),
  m_mutex(mutex),
  m_cv(cv)
{};



void* ReplicaHandler::run() {
  printf("[Replica] Started a new connection\n");
  
  Packet* request;
  while((request = m_connection->receive()) != NULL) {
    if (request->type() == ELECTION) {
      if (m_isRunningElection.get() == false)
        m_isRunningElection.set(true);
      
      std::string payload = request->payload();
      if (payload.size() > 0) {
        printf("[Replica] Received ELECTION from %s\n", payload.c_str());
        ReplicaInfo replica = ReplicaInfo::deserialize(payload.c_str());

        printf("You:<%s> Me:<%s>\n", replica.serialize().c_str(), m_info.serialize().c_str());
        bool isNotEqualToMe = replica.ip != m_info.ip || replica.port != m_info.port;
        bool isGreaterThanMe = replica.id < m_info.id; // Greater id has less value
        if (isNotEqualToMe && isGreaterThanMe) {
          printf("You won!\n");
          Packet answerPacket = Packet(ANSWER, "Won");
          m_connection->send(&answerPacket);
        } else {
          printf("You lost!\n");
          Packet answerPacket = Packet(ANSWER, "Lost");
          m_connection->send(&answerPacket);
        }
      }

    } else if (request->type() == COORDINATOR) {
      std::string payload = request->payload();
      if (payload.size() > 0) {
        printf("[Replica] Received COORDINATOR from %s\n", payload.c_str());
        m_isConnectedToLeader = true;
        //m_isRunningElection.set(false);
      }

    } else if (request->type() == ANSWER) {
      m_mutex.lock();
      if (std::string(request->payload()) == "Lost") {
        m_lostElection.set(true);
      }
      m_mutex.unlock();
      m_cv.notify_one();
    }

    delete request; request = NULL;
  }

  // If it reaches this code, then the connection was lost!
  if (m_isConnectedToLeader)
    m_isRunningElection.set(true);

  m_connection->close(); // Ensure it's closed

  printf("[Replica] Connection ended.\n");
  return NULL;
}
