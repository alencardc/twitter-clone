#include "Session.hpp"

Session::Session(long unsigned int threadId, std::string username) {
  m_threadId = threadId;
  m_username = username;
}

long unsigned int Session::id() {
  return m_threadId;
}