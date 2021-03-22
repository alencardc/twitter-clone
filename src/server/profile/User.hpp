#ifndef __User_hpp__
#define __User_hpp__

#include <string>
#include <list>
#include "Session.hpp"

#define MAX_SESSIONS_PER_USER 2

class User {
  int m_id;
  std::string m_username;
  std::list<std::string> m_followers;
  std::list<Session> m_sessions;

  public:
    User();
    User(int id, std::string username);
    User(int id, std::string username, std::list<std::string> followers);
    ~User();

    int id();
    std::string username();
    std::list<std::string> followers();

    int sessionCount();
    std::list<Session> sessions();
    bool addSession(Session session);
    void removeSession(long unsigned int id);
};

#endif // __User_hpp__