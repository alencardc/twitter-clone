#ifndef __SyncAccess_hpp__
#define __SyncAccess_hpp__

#include <queue>
#include <mutex>
#include <tuple>
#include <chrono>
#include <condition_variable>

template <typename T>
class SyncAccess {
  T m_data;

  std::mutex m_mutex;

  public: 
    void set(T data) {
      m_mutex.lock();
      m_data = data;
      m_mutex.unlock();
    }

    // WARNING: It is unsafe in case the type T is a point or contains pointer fields
    T get() {
      m_mutex.lock();
      T value = m_data;
      m_mutex.unlock();
      return value;
    }

    // Type T must have a copy constructor implementing deep copy
    T* getDeepCopy() { 
      m_mutex.lock();
      T* value = new T(m_data);
      m_mutex.unlock();
      return value;
    }


};

#endif // __SyncAccess_hpp__