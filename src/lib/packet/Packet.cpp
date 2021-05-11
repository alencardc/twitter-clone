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
  m_length = length;
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

  std::string strPacket = rawPacket.str();
  uint32_t totalSize = strPacket.size();
  char buffSize[10 + 1]; // Size to support max value of uint32
  sprintf(buffSize, "%010u", totalSize);
  strPacket.insert(0, buffSize);

  return strPacket;
}

Packet* Packet::deserialize(const char* rawData) {
  int length;
  int sequenceNumber;
  long unsigned int timestamp;
  char typeBuffer[30];

  std::string strData = rawData;
  strData.erase(0, sizeFieldLength);

  std::vector<std::string> headerAndPayload = splitFirst(strData, "\n\n");
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
    case ANSWER:
      return "ANSWER";
    case ACK:
      return "ACK";
    case NEW_REPLICA:
      return "NEW_REPLICA";
    case REPLICAS:
      return "REPLICAS";
    case IS_ALIVE:
      return "IS_ALIVE";
    case SERVER_DOWN:
      return "SERVER_DOWN";
    case ELECTION:
      return "ELECTION";
    case NEW_LEADER:
      return "NEW_LEADER";
    case COORDINATOR:
      return "COORDINATOR";
    case UPDATE_PROFILE:
      return "UPDATE_PROFILE";
    case UPDATE_NOTIFICATION:
      return "UPDATE_NOTIFICATION";
    case SEND:
      return "SEND";
    case FOLLOW:
      return "FOLLOW";
    case LOGIN:
      return "LOGIN";
    case SUCCESS:
      return "SUCCESS";
    case ERROR:
      return "ERROR";
    case DATA:
      return "DATA";
    case CLOSE:
      return "CLOSE";
    default:
      return "NO_TYPE";
  }
}

PacketType Packet::stringToType(const char* buffer) {
  std::string s = buffer;
  if (s == "ACK")
    return ACK;
  else if (s == "ANSWER")
    return ANSWER;
  else if (s == "NEW_REPLICA")
    return NEW_REPLICA;
  else if (s == "REPLICAS")
    return REPLICAS;
  else if (s == "IS_ALIVE")
    return IS_ALIVE;
  else if (s == "SERVER_DOWN")
    return SERVER_DOWN;
  else if (s == "ELECTION")
    return ELECTION;
  else if (s == "NEW_LEADER")
    return NEW_LEADER;
  else if (s == "COORDINATOR")
    return COORDINATOR;
  else if (s == "UPDATE_PROFILE")
    return UPDATE_PROFILE;
  else if (s == "UPDATE_NOTIFICATION")
    return UPDATE_NOTIFICATION;
  else if (s == "SEND")
    return SEND;
  else if (s == "FOLLOW")
    return FOLLOW;
  else if (s == "LOGIN")
    return LOGIN;
  else if (s == "SUCCESS")
    return SUCCESS;
  else if (s == "ERROR")
    return ERROR;
  else if (s == "DATA")
    return DATA;
  else if (s == "CLOSE")
    return CLOSE;
  else 
    return NO_TYPE;
}