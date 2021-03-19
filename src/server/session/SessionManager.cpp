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

bool SessionManager::startSession(std::string username, int session) {
  m_mutex.lock();

  bool success = false;
  if (m_sessions.count(username) == 1 && m_sessions[username].size() < 2) {
    m_sessions[username].push_back(session);
    success = true;
  } else {
    std::list<int> userSessions;
    userSessions.push_back(session);
    m_sessions.emplace(username, session);
    success = true;
  }

  m_mutex.unlock();

  return success;
}

void SessionManager::closeSession(std::string username, int id) {
  m_mutex.lock();

  if (m_sessions.count(username) == 1) {
    if (m_sessions[username].front() == id) {
      m_sessions[username].pop_front();
    } else if (m_sessions[username].back() == id) {
      m_sessions[username].pop_back();
    }
  }

  m_mutex.unlock();
}