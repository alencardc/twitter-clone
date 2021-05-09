#ifndef __Notification_hpp__
#define __Notification_hpp__

#include <string>
#include "lib/packet/Serializable.hpp"

class Notification : public Serializable {
  public: 
    int id;
    long unsigned timestamp;
    std::string username;
    int pendingCount;
    std::string message;

    Notification();

    std::string serialize();
    std::string serializeAll();
    void deserializeAll(std::string rawData);
    Notification* deserialize(const char* rawData);
};

#endif // __Notification_hpp__