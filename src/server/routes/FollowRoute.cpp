#include "FollowRoute.hpp"

FollowRoute::FollowRoute(Packet& request):
  m_request(request)
{}

Packet* FollowRoute::execute() {
  return new Packet(OK, "TODO: Follow users");
}