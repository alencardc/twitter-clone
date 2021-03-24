#include "PendingNotification.hpp"

PendingNotification::PendingNotification(): sender(""), notificationId(-1) {}

PendingNotification::PendingNotification(std::string username, int id):
  sender(username),
  notificationId(id)
{}