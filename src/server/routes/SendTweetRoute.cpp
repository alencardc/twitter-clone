#include "SendTweetRoute.hpp"

SendTweetRoute::SendTweetRoute(Packet& request):
  m_request(request)
{}

Packet* SendTweetRoute::execute() {
  return new Packet(OK, "TODO: Send tweet to followers");
}