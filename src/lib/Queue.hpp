#ifndef __Queue_hpp__
#define __Queue_hpp__

#include <queue>
#include <mutex>
#include <tuple>
#include <chrono>
#include <condition_variable>

template <typename T>
class Queue {
  std::queue<T> m_queue;

  std::mutex m_mutex;
  std::condition_variable m_cv;

  public: 

    void insert(T element) {
      m_mutex.lock();

      m_queue.push(element);
      
      m_mutex.unlock();
      m_cv.notify_one(); // Wake up just one thread waiting for cv.
    }

    T remove() {
      std::unique_lock<std::mutex> lock(m_mutex); // Locks mutex (m_mutex.lock())
      //m_mutex.lock();
    
      while (m_queue.empty() == true) {
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

#endif // __Queue_hpp__