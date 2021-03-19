#include <unistd.h>
#include <iostream>
#include <string>
#include "lib/Thread.hpp"
#include "server/session/SessionManager.hpp"

class SomeThread : public Thread {
  int m_id;
  SessionManager& m_manager;

  public:
    SomeThread(int id, SessionManager& manager) : m_id(id), m_manager(manager) {}

    void* run() {
      m_manager.startSession("alencar", 1);
      printf("[%ld] Size:%d\n", getId(), m_manager.activeSessionCount("alencar"));
      m_manager.startSession("alencar", 2);
      printf("[%ld] Size:%d\n", getId(), m_manager.activeSessionCount("alencar"));
      m_manager.startSession("alencar", 3);
      printf("[%ld] Size:%d\n", getId(), m_manager.activeSessionCount("alencar"));
      m_manager.closeSession("alencar", 1);
      printf("[%ld] Size:%d\n", getId(), m_manager.activeSessionCount("alencar"));
      m_manager.closeSession("alencar", 2);
      printf("[%ld] Size:%d\n", getId(), m_manager.activeSessionCount("alencar"));
      

      return NULL;
    }
};

int main() {
  SessionManager manager;

  SomeThread* thread1 = new SomeThread(1, manager);
  SomeThread* thread2 = new SomeThread(2, manager);
  thread1->start();
  thread2->start();

  
  while(1);
  delete thread1;
  delete thread2;

  return 0;
}