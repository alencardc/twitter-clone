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

std::string Notification::serializeAll() {
  std::stringstream stream;
  stream << id << " " << timestamp << " " << username << " " << pendingCount;
  stream << " " << message.size() << " " << message.c_str();

  return stream.str();
}

void Notification::deserializeAll(std::string rawData) {
  std::istringstream stream(rawData);

  stream >> id;
  stream >> timestamp;
  stream >> username;
  stream >> pendingCount;
  int length;
  stream >> length;
  char msgBuffer[length];
  stream.ignore(1);
  stream.read(msgBuffer, length);
  msgBuffer[length] = '\0';
  message = msgBuffer;
}

Notification* Notification::deserialize(const char* rawData) {
  return NULL;
}