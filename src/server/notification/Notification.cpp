#include <sstream>
#include <time.h>
#include "Notification.hpp"

Notification::Notification() :
  id(-1),
  timestamp(time(NULL)),
  username(""),
  pendingCount(-1),
  message("")
{}

std::string Notification::serialize() {
  std::stringstream rawPacket;
  rawPacket << "id:" << id;
  rawPacket << "\ntimestamp:" << timestamp;
  rawPacket << "\nsender:" << username;
  rawPacket << "\nlength:" << message.size();
  rawPacket << "\n\n" << message.c_str();

  return rawPacket.str();
}

Notification* Notification::deserialize(const char* rawData) {
  return NULL;
}