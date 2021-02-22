#include "Thread.hpp"

Thread::Thread() {
}

Thread::~Thread() {
  if (thread->joinable()) {
    thread->detach();
  }
  delete thread;
}

void* Thread::runThread(Thread* instance) {
  return instance->run();
}

bool Thread::start() {
  thread = new std::thread(runThread, this);

  return thread->joinable();
}

bool Thread::join() {
  if (thread->joinable()) {
    thread->join();
  }
  return !thread->joinable();
}

std::thread::id Thread::getId() {
  return thread->get_id();
}