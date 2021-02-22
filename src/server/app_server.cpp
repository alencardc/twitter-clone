#include <iostream>

#include "lib/Thread.hpp"

class TestThread : public Thread {
  private:
    void* run() {
      std::cout << "Hello Thread" << std::endl;
      return NULL;
    }
};

int main() {
  std::cout << "Hello, World!" << std::endl;
  TestThread* thread1 = new TestThread();
  thread1->start();
  thread1->join();
  return 0;
}