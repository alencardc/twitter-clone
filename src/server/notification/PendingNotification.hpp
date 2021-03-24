#ifndef __PendingNotification_hpp__
#define __PendingNotification_hpp__

#include <string>

class PendingNotification {
  public:
    std::string sender;
    int notificationId;

    PendingNotification(std::string username, int id);
};

#endif // __PendingNotification_hpp__