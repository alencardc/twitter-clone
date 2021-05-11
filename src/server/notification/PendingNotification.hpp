#ifndef __PendingNotification_hpp__
#define __PendingNotification_hpp__

#include <string>

class PendingNotification {
  public:
    std::string sender;
    int notificationId;

    PendingNotification();
    PendingNotification(std::string username, int id);

    std::string serialize();
    void deserialize(std::string raw);
};

#endif // __PendingNotification_hpp__