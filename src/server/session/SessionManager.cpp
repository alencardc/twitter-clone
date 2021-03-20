#include "SessionManager.hpp"

SessionManager::SessionManager() {}
SessionManager::~SessionManager() {}

int SessionManager::activeSessionCount(std::string username) {
  m_mutex.lock();

  int count = 0;
  if (m_sessions.count(username) == 1) {
    count = m_sessions[username].size();
  }

  m_mutex.unlock();

  return count;
}

bool SessionManager::startSession(std::string username, Session session) {
  m_mutex.lock();

  bool success = false;
  if (m_sessions.count(username) == 1) {
    if (m_sessions[username].size() < 2) {
      m_sessions[username].push_back(session);
      success = true;
    }
  } else {
    std::list<Session> userSessions;
    userSessions.push_back(session);
    m_sessions.emplace(username, userSessions);
    success = true;
  }

  m_mutex.unlock();

  return success;
}

void SessionManager::closeSession(std::string username, long unsigned int id) {
  m_mutex.lock();

  if (m_sessions.count(username) == 1) {
    if (m_sessions[username].front().id() == id) {
      m_sessions[username].pop_front();
    } else if (m_sessions[username].back().id() == id) {
      m_sessions[username].pop_back();
    }
  }

  m_mutex.unlock();
}