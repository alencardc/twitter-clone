#ifndef __Session_Manager_hpp__
#define __Session_Manager_hpp__

#include <mutex>
#include <unordered_map>
#include <list>

class SessionManager {
  std::unordered_map<std::string, std::list<int>> m_sessions;

  std::mutex m_mutex;

  public:
    SessionManager();
    ~SessionManager();

    int activeSessionCount(std::string username);
    bool startSession(std::string username, int session);
    void closeSession(std::string username, int id);
};

#endif // __Session_Manager_hpp__