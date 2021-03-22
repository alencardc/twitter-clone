#include "FollowRoute.hpp"

FollowRoute::FollowRoute(Packet& request, ProfileManager& manager):
  m_request(request),
  m_manager(manager)
{}

Packet* FollowRoute::execute() {
  

  return new Packet(OK, "TODO: Follow users");
}