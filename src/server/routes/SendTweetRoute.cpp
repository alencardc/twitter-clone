#include "SendTweetRoute.hpp"

SendTweetRoute::SendTweetRoute(
  std::string username,
  Packet& request,
  NotificationManager& notificationManager,
  ProfileManager& profileManager
):
  m_username(username),
  m_request(request),
  m_notificationManager(notificationManager),
  m_profileManager(profileManager)
{}

Packet* SendTweetRoute::execute() {
  std::string message = m_request.payload();
  Notification notification;
  notification.message = message;
  notification.username = m_username;

  std::list<std::string> followers = m_profileManager.followersOf(m_username);
  notification.pendingCount = followers.size();

  m_notificationManager.send(notification, followers);

  return new Packet(SUCCESS, "Sent!");
}