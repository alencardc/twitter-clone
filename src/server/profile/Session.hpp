#ifndef __Session_hpp__
#define __Session_hpp__

#include <string>

class Session {
  long unsigned int m_threadId;
  std::string m_username;

  public:
    Session(long unsigned int threadId, std::string username);
    long unsigned int id();

    std::string serialize();
};

#endif // __Session_hpp__