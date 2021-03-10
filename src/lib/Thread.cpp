#include "Thread.hpp"

Thread::Thread() {
}

Thread::~Thread() {
  if (m_thread->joinable()) {
    m_thread->detach();
  }
  delete m_thread;
}

void* Thread::runThread(Thread* instance) {
  return instance->run();
}

bool Thread::start() {
  m_thread = new std::thread(runThread, this);

  return m_thread->joinable();
}

bool Thread::join() {
  if (m_thread->joinable()) {
    m_thread->join();
  }
  return !m_thread->joinable();
}

long unsigned int Thread::getId() {
  return (long unsigned int)m_thread->native_handle();
}