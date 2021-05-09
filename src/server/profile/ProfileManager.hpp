#ifndef __ProfileManager_hpp__
#define __ProfileManager_hpp__

#include <list>
#include <mutex>
#include <vector>
#include <functional>
#include <unordered_map>
#include "User.hpp"

class ProfileManager {
  std::unordered_map<std::string, User> m_users;
  std::mutex m_mutex;


  public:
    std::function<bool(ProfileManager&)> onUpdateCallback;
    ProfileManager();
    ~ProfileManager();

    // Load users from persistency
    void loadUsers();
    std::vector<std::string> getAllUsernames();

    bool userExists(std::string username);
    bool follow(std::string follower, std::string userToFollow);
    std::list<std::string> followersOf(std::string username);

    int activeSessionCount(std::string username);
    bool startSession(std::string username, Session session);
    void closeSession(std::string username, long unsigned int id);

    void update(ProfileManager& manager);
    std::string serialize();
    void deserialize(std::string raw);
};

#endif // __ProfileManager_hpp__
