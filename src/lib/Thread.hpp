#ifndef __Thread_hpp__
#define __Thread_hpp__

#include <thread>

class Thread {
  private: 
    std::thread* thread;

  public:
    Thread();
    virtual ~Thread();

    bool start();
    bool join();
    std::thread::id getId();
    static void* runThread(Thread* instance);
  
  private:
    virtual void* run() = 0;
};

#endif // __Thread_hpp__