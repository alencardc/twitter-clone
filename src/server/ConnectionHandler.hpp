#ifndef __CONNECTION_HANDLER_HPP__
#define __CONNECTION_HANDLER_HPP__

#include "lib/Thread.hpp"
#include "lib/socket/WorkQueue.hpp"
#include "server/WorkItem.hpp"

class ConnectionHandler : public Thread {
  WorkQueue<WorkItem*>& m_queue;

  public: 
    ConnectionHandler(WorkQueue<WorkItem*>& queue);

  private: 
    void* run();

};


#endif // __CONNECTION_HANDLER_HPP__