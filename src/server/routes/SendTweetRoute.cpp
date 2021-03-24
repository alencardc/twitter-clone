#include "SendTweetRoute.hpp"

SendTweetRoute::SendTweetRoute(std::string username, Packet& request, NotificationManager& manager):
  m_username(username),
  m_request(request),
  m_manager(manager)
{}

Packet* SendTweetRoute::execute() {
  std::string message = m_request.payload();
  Notification notification;
  notification.id = 1;
  notification.message = message;
  notification.pendingCount = 10;
  notification.timestamp = 0;
  notification.username = m_username;

  std::list<std::string> followers;
  followers.push_back("alencar1");
  followers.push_back("alencar2");
  followers.push_back("alencar3");
  m_manager.send(notification, followers);

  return new Packet(OK, ("Sent message: " + message).c_str());
}