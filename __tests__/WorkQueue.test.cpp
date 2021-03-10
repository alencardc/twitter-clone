#include <unistd.h>
#include <iostream>
#include <string>
#include "lib/Thread.hpp"
#include "lib/socket/WorkQueue.hpp"

class SomeConsumer : public Thread {
  int m_id;
  WorkQueue<std::string*>& m_queue;

  public:
    SomeConsumer(int id, WorkQueue<std::string*>& queue) : m_id(id), m_queue(queue){}

    void* run() {
      for (int i = 0; i < 10; i++) {
        printf("Consumer thread %d - waiting for item %d\n", m_id, i);
        std::string* item = m_queue.pop();
        printf("Consumer thread %d - got item %d with message: %s\n", m_id, i, item->c_str());
        delete item;
      }
      return NULL;
    }
};

int main() {
  WorkQueue<std::string*> queue;

  SomeConsumer* thread1 = new SomeConsumer(1, queue);
  SomeConsumer* thread2 = new SomeConsumer(2, queue);
  thread1->start();
  thread2->start();

  std::string* item;
  for (int i = 0; i < 5; i++) {
    item = new std::string("Message 1");
    queue.push(item);
    printf("Produced\n");
    item = new std::string("Message 2");
    queue.push(item);
    printf("Produced\n");
    item = new std::string("Message 3");
    queue.push(item);
    printf("Produced\n");
    sleep(1);
  }
  printf("Finished produced!");
  while(1);
  delete thread1;
  delete thread2;

  return 0;
}