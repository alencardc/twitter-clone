#ifndef __Queue_hpp__
#define __Queue_hpp__

#include <deque>
#include <mutex>
#include <tuple>
#include <chrono>
#include <condition_variable>

template <typename T>
class Queue {
  std::deque<T> m_queue;

  std::mutex m_mutex;
  std::condition_variable m_cv;

  public: 

    void insert(T element) {
      m_mutex.lock();
      
      m_queue.push_back(element);
      
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
      m_queue.pop_front();
      lock.unlock();

      return element;
    }

    std::pair<bool, T> tryRemove() {
      std::pair<bool, T> element = std::make_pair(false, T());

      if (m_mutex.try_lock()) {
        if (m_queue.empty() != true) {
          element = std::make_pair(true, m_queue.front());
          m_queue.pop_front();
        }
        m_mutex.unlock();
      }
      return element;
    }

    std::pair<bool, T> tryRemoveFor(unsigned milliseconds) {
      std::unique_lock<std::mutex> lock(m_mutex); // Locks mutex (m_mutex.lock())
      //m_mutex.lock();
      std::cv_status status = std::cv_status::no_timeout;
      while (m_queue.empty() == true && status == std::cv_status::no_timeout) {
        status = m_cv.wait_for(lock, std::chrono::milliseconds(milliseconds));
      }

      std::pair<bool, T> element = std::make_pair(false, T());
      if (status == std::cv_status::no_timeout) {
        element = std::make_pair(true, m_queue.front());
        m_queue.pop_front();
      } 
      lock.unlock();

      return element;
    }

    int size() {
      m_mutex.lock();
      int size = m_queue.size();
      m_mutex.unlock();

      return size;
    }

    std::deque<T> get() const {
      return m_queue;
    }

    void set(std::deque<T> queue) {
      m_queue = queue;
    }

};

#endif // __Queue_hpp__