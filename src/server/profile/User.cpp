#include <algorithm>
#include "User.hpp"

User::User() {
  m_id = -1;
  m_username = "";
}

User::User(int id, std::string username): m_id(id), m_username(username) {}

User::User(int id, std::string username, std::list<std::string> followers):
  m_id(id),
  m_username(username),
  m_followers(followers)
{}

User::~User() {}

int User::id() {
  return m_id;
}

std::string User::username() {
  return m_username;
}

// Check if reference or copy
std::list<std::string> User::followers() {
  return m_followers;
}

bool User::follow(std::string userToFollow) {
  if (m_username == userToFollow) {
    return false;
  }

  m_followers.sort();
  bool alreadyFollow = std::binary_search(
    m_followers.begin(),
    m_followers.end(),
    userToFollow);

  if (alreadyFollow == true) {
    return false;
  }

  m_followers.push_back(userToFollow);
  return true;
}

int User::sessionCount() {
  return m_sessions.size();
}

std::list<Session> User::sessions() {
  return m_sessions;
}

bool User::addSession(Session session) {
  if (m_sessions.size() < MAX_SESSIONS_PER_USER) {
    m_sessions.push_back(session);
    return true;
  }

  return false;
}

void User::removeSession(long unsigned int id) {
  if (m_sessions.size() > 0) {
    if (m_sessions.front().id() == id) {
      m_sessions.pop_front();
    } else if (m_sessions.back().id() == id) {
      m_sessions.pop_back();
    }
  }
}