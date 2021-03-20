#ifndef __Session_Manager_hpp__
#define __Session_Manager_hpp__

#include <mutex>
#include <unordered_map>
#include <list>
#include "Session.hpp"

class SessionManager {
  std::unordered_map<std::string, std::list<Session>> m_sessions;

  std::mutex m_mutex;

  public:
    SessionManager();
    ~SessionManager();

    int activeSessionCount(std::string username);
    bool startSession(std::string username, Session session);
    void closeSession(std::string username, long unsigned int id);
};

#endif // __Session_Manager_hpp__