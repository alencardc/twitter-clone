#ifndef __ClientNotification_hpp__
#define __ClientNotification_hpp__

#include <string>

class Notification {
  public:
    int id;
    std::string sender;
    std::string date;
    std::string message;

    static const long unsigned int MAX_SIZE = 128;

    Notification();
    Notification(int pId, std::string pSender, std::string pDate, std::string pMessage);

    void setDate(long unsigned timestamp);

    static Notification* deserialize(std::string raw);
};

#endif // __ClientNotification_hpp__