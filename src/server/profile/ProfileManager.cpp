#include "ProfileManager.hpp"

ProfileManager::ProfileManager() {}
ProfileManager::~ProfileManager() {}

int ProfileManager::activeSessionCount(std::string username) {
  m_mutex.lock();

  int count = 0;
  if (m_users.count(username) == 1) {
    count = m_users[username].sessionCount();
  }

  m_mutex.unlock();

  return count;
}

bool ProfileManager::startSession(std::string username, Session session) {
  m_mutex.lock();

  bool success = false;
  if (m_users.count(username) == 1) {
    success = m_users[username].addSession(session);
  } else {
    User newUser = User(m_users.size(), username);
    newUser.addSession(session);
    m_users.emplace(username, newUser);
    success = true;
  }

  m_mutex.unlock();

  return success;
}

void ProfileManager::closeSession(std::string username, long unsigned int id) {
  m_mutex.lock();

  if (m_users.count(username) == 1) {
    m_users[username].removeSession(id);
  }

  m_mutex.unlock();
}