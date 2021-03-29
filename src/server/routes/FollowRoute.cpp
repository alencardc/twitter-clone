#include "FollowRoute.hpp"

FollowRoute::FollowRoute(std::string username, Packet& request, ProfileManager& manager):
  m_username(username),
  m_request(request),
  m_manager(manager)
{}

Packet* FollowRoute::execute() {
  std::string follow = m_request.payload();

  if (m_manager.follow(m_username, follow) == true) {
    return new Packet(SUCCESS, ("Now you are following " + follow).c_str());
  }

  return new Packet(SUCCESS, ("It was not possible to follow " + follow).c_str());
}