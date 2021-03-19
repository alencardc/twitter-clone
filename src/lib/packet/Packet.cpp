#include <ctime>
#include <string.h>
#include <sstream>
#include "Packet.hpp"

Packet::Packet(PacketType type, Serializable* data) {
  m_type = type;
  m_timestamp = std::time(NULL);
  m_sequenceNumber = -1;

  std::string payload = data->serialize();
  m_payload = strdup(payload.c_str());
  m_length = payload.size();
}

Packet::Packet(PacketType type, const char* payload) {
 m_type = type;
  m_timestamp = std::time(NULL);
  m_sequenceNumber = -1;

  m_payload = strdup(payload);
  m_length = strlen(payload + 1);
}

Packet::~Packet() {
  delete m_payload;
}

std::string Packet::serialize() {
  std::stringstream rawPacket;
  rawPacket << "type:" << typeToString(m_type) ;
  rawPacket << "\ntimestamp:" << m_timestamp;
  rawPacket << "\nsequenceNumber:" << m_sequenceNumber;
  rawPacket << "\nlength:" << m_length;
  rawPacket << "\npayload:" << m_payload;
  rawPacket << "\n";

  return rawPacket.str();
}

const char* Packet::typeToString(PacketType type) {
  switch (type) {
    case COMMAND:
      return "COMMAND";
    case DATA:
      return "DATA";
    default:
      return "";
  }
}