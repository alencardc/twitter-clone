#include <ctime>
#include <string.h>
#include <sstream>
#include <vector>
#include "Packet.hpp"
#include "lib/utils/string.hpp"

Packet::Packet(PacketType type, Serializable* data) {
  m_type = type;
  m_timestamp = std::time(NULL);
  m_sequenceNumber = -1;


  std::string payload = data == NULL ? "" : data->serialize();
  m_payload = strdup(payload.c_str());
  m_length = payload.size();
}

Packet::Packet(PacketType type, const char* payload) {
  m_type = type;
  m_timestamp = std::time(NULL);
  m_sequenceNumber = -1;

  std::string buff = payload;
  m_payload = strdup(buff.c_str());
  m_length = buff.size();
}

Packet::Packet(int length, PacketType type, int sequenceNumber, long unsigned int timestamp, const char* payload) {
  m_type = type;
  m_timestamp = timestamp;
  m_sequenceNumber = sequenceNumber;

  std::string s = payload;
  m_payload = strdup(payload);
  m_length = s.size();//strlen(payload + 1);
}

Packet::~Packet() {
  delete m_payload;
}

const char* Packet::payload() {
  return m_payload;
}

int Packet::length() {
  return m_length;
}

PacketType Packet::type() {
  return m_type;
}

std::string Packet::typeString() {
  return typeToString(m_type);
}

std::string Packet::serialize() {
  std::stringstream rawPacket;
  rawPacket << "type:" << typeToString(m_type) ;
  rawPacket << "\ntimestamp:" << m_timestamp;
  rawPacket << "\nsequenceNumber:" << m_sequenceNumber;
  rawPacket << "\nlength:" << m_length;
  rawPacket << "\n\n" << m_payload;

  return rawPacket.str();
}

Packet* Packet::deserialize(const char* rawData) {
  int length;
  int sequenceNumber;
  long unsigned int timestamp;
  char typeBuffer[30];

  std::vector<std::string> headerAndPayload = splitFirst(rawData, "\n\n");
  if (headerAndPayload.size() == 2) {
    int amountRead = sscanf(
      headerAndPayload[0].c_str(), 
      "type:%s\ntimestamp:%lu\nsequenceNumber:%d\nlength:%d",
      typeBuffer, &timestamp, &sequenceNumber, &length);
    
    if (amountRead == 4) {
      const char* payload = headerAndPayload[1].c_str();
      return new Packet(length, stringToType(typeBuffer), sequenceNumber, timestamp, payload);
    }
  }
  
  return new Packet(NO_TYPE, "Unable to parse. Bad request.");
}

const char* Packet::typeToString(PacketType type) {
  switch (type) {
    case SEND:
      return "SEND";
    case FOLLOW:
      return "FOLLOW";
    case LOGIN:
      return "LOGIN";
    case OK:
      return "OK";
    case ERROR:
      return "ERROR";
    case DATA:
      return "DATA";
    default:
      return "NO_TYPE";
  }
}

PacketType Packet::stringToType(const char* buffer) {
  std::string s = buffer;
  if (s == "SEND")
    return SEND;
  else if (s == "FOLLOW")
    return FOLLOW;
  else if (s == "LOGIN")
    return LOGIN;
  else if (s == "OK")
    return OK;
  else if (s == "ERROR")
    return ERROR;
  else if (s == "DATA")
    return DATA;
  else 
    return NO_TYPE;
}