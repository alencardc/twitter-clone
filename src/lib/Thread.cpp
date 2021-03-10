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

std::thread::id Thread::getId() {
  return m_thread->get_id();
}