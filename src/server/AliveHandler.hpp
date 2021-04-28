#ifndef __AliveHandler_hpp__
#define __AliveHandler_hpp__

#include <string>
#include "lib/Thread.hpp"
#include "lib/SyncAccess.hpp"

class AliveHandler : public Thread {
  SyncAccess<bool>& m_isLeader;
  SyncAccess<bool>& m_election;
  std::string m_ownerIp;
  int m_ownerPort;

  public:
    AliveHandler(SyncAccess<bool>& isLeader, SyncAccess<bool>& election, std::string ownerIp, int ownerPort);
  private:
    void* run();

};


#endif // __AliveHandler_hpp__