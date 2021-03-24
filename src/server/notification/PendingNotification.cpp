#include "PendingNotification.hpp"

PendingNotification::PendingNotification(std::string username, int id):
  sender(username),
  notificationId(id)
{}