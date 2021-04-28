#ifndef __ElectionHandler_hpp__
#define __ElectionHandler_hpp__

#include <vector>
#include "lib/Thread.hpp"
#include "server/ReplicaInfo.hpp"

// Handles all the process of election using Bully Algorithm
class ElectionHandler : public Thread {
  ReplicaInfo m_ownerInfo;
  std::vector<ReplicaInfo> m_replicas;

  public: 
    ElectionHandler(ReplicaInfo ownerInfo, std::vector<ReplicaInfo> replicas);

  private:
    void* run();
};

#endif // __ElectionHandler_hpp__