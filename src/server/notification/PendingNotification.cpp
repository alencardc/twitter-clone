#include <sstream>
#include "PendingNotification.hpp"

PendingNotification::PendingNotification(): sender(""), notificationId(-1) {}

PendingNotification::PendingNotification(std::string username, int id):
  sender(username),
  notificationId(id)
{}

std::string PendingNotification::serialize() {
  return sender + " " + std::to_string(notificationId);
}

void PendingNotification::deserialize(std::string raw) {
  std::istringstream stream(raw);
  stream >> sender;
  stream >> notificationId;
}