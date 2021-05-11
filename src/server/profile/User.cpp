#include <algorithm>
#include "lib/utils/string.hpp"
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

bool User::addFollower(std::string follower) {
  if (m_username == follower) {
    return false;
  }

  m_followers.sort();
  bool alreadyFollow = std::binary_search(
    m_followers.begin(),
    m_followers.end(),
    follower);

  if (alreadyFollow == true) {
    return false;
  }

  m_followers.push_back(follower);
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

bool User::isValid() {
  return m_id > 0 && m_username.size() > 0;
}

std::string User::serialize() {
  std::string s;
  s.append(std::to_string(m_id) + " " + m_username + " ");
  s.append(std::to_string(m_followers.size()) + " ");
  s.append(joinStringList(m_followers, ","));
  s.append(" " + std::to_string(m_sessions.size()) + " ");
  bool first = true;
  for (Session session : m_sessions) {
    if (first) {
      first = false;
      s.append(session.serialize());
    } else
      s.append("," + session.serialize());
  }
  return s;
}

void User::deserialize(std::string raw) {
  std::vector<std::string> splits = split(raw, " ");
  if (splits.size() < 6)
    return;
  
  m_id = std::stoi(splits[0]);
  m_username = splits[1];

  int followersSize = std::stoi(splits[2]);
  if (followersSize > 0) {
    m_followers.clear();
    std::vector<std::string> followers = split(splits[3], ",");
    for (unsigned i = 0; i < followers.size(); i++) {
      m_followers.push_back(followers[i]);
    }
  }
  
  // Sessions
}