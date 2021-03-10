#ifndef __WorkQueue_hpp__
#define __WorkQueue_hpp__
#include <stdio.h>
#include <queue>
#include <mutex>
#include <condition_variable>

using namespace std;

template <typename T>
class WorkQueue {
  queue<T> m_queue;
  mutex m_mutex;
  condition_variable m_cv;

  public:
    WorkQueue() {}
    ~WorkQueue() {}

    void push(T element) {
      m_mutex.lock();

      m_queue.push(element);
      
      m_cv.notify_one(); // Wake up just one thread waiting for cv.
      m_mutex.unlock();
    }
    T pop() {
      unique_lock<mutex> lock(m_mutex); // Locks mutex (m_mutex.lock())
      //m_mutex.lock();

      while (m_queue.size() == 0) {
        m_cv.wait(lock);
      }
      T element = m_queue.front();
      m_queue.pop();
      lock.unlock();

      return element;
    }

    int size() {
      m_mutex.lock();
      int size = m_queue.size();
      m_mutex.unlock();

      return size;
    }
};

#endif // __WorkQueue_hpp__