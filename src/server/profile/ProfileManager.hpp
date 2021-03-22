#ifndef __ProfileManager_hpp__
#define __ProfileManager_hpp__

#include <list>
#include <mutex>
#include <unordered_map>
#include "User.hpp"

class ProfileManager {
  std::unordered_map<std::string, User> m_users;
  std::mutex m_mutex;

  public:
    ProfileManager();
    ~ProfileManager();

    bool userExists(std::string username);
    bool follow(std::string follower, std::string userToFollow);

    int activeSessionCount(std::string username);
    bool startSession(std::string username, Session session);
    void closeSession(std::string username, long unsigned int id);
};

#endif // __ProfileManager_hpp__
