#include "ProfileManager.hpp"
#include "ProfilePersistency.hpp"
#include "lib/utils/string.hpp"

ProfileManager::ProfileManager() {}
ProfileManager::~ProfileManager() {}

void ProfileManager::loadUsers() {
  m_mutex.lock();

  ProfilePersistency db;
  m_users = db.readUsers();
  
  m_mutex.unlock();
}

bool ProfileManager::userExists(std::string username) {
  m_mutex.lock();

  bool exists = m_users.count(username) == 1;

  m_mutex.unlock();
  return exists;
}

bool ProfileManager::follow(std::string follower, std::string userToFollow) {
  m_mutex.lock();

  bool success = false;
  if (m_users.count(follower) == 1 && m_users.count(userToFollow) == 1) {
    success = m_users[userToFollow].addFollower(follower);
    if (success) {
      ProfilePersistency db;
      db.saveNewFollower(userToFollow, follower);
      success = onUpdateCallback(*this);
    }
  }
  m_mutex.unlock();
  return success;
}

std::list<std::string> ProfileManager::followersOf(std::string username) {
  m_mutex.lock();

  std::list<std::string> followers;
  if (m_users.count(username) == 1) {
    followers = m_users[username].followers();
  }

  m_mutex.unlock();

  return followers;
}

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
  } else { // New user!
    User newUser = User(m_users.size(), username);
    newUser.addSession(session);
    m_users.emplace(username, newUser);

    ProfilePersistency db;
    db.saveNewUser(username);
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

void ProfileManager::update(ProfileManager& manager) {
  m_users = manager.m_users;
  for (auto u : m_users) {
    printf("Update -> user: %s\n", u.first.c_str());
  }
}

std::string ProfileManager::serialize() {
  std::string s;
  for (auto user : m_users) {
    s.append(user.first + " " + user.second.serialize() + "\n");
  }
  return s;
}

void ProfileManager::deserialize(std::string raw) {
  std::unordered_map<std::string, User> users;
  std::vector<std::string> rawUsers = split(raw, "\n");
  for (std::string rawUser : rawUsers) {
    if (rawUser.size() > 0) {
      std::vector<std::string> pair = splitFirst(rawUser, " ");
      if (pair.size() == 2) {
        User user;
        user.deserialize(pair[1]);
        if (user.isValid())
          users.emplace(pair[0], user);
      }
    }
  }
  m_users = users;
}