#ifndef __Packet_hpp__
#define __Packet_hpp__

#include <string>
#include "Serializable.hpp"

enum PacketType {
  ACK,
  IS_ALIVE,
  NEW_REPLICA,
  REPLICAS,
  ELECTION,
  DATA,
  SEND,
  LOGIN,
  FOLLOW,
  SUCCESS,
  ERROR,
  NO_TYPE,
};

class Packet {
  public:
  int m_length;
  PacketType m_type;
  int m_sequenceNumber;
  long unsigned int m_timestamp;
  const char* m_payload;

  public:
    friend class TCPConnection;

    Packet(PacketType type, Serializable* data);
    Packet(PacketType type, const char* payload);
    ~Packet();

    const char* payload();
    int length();
    PacketType type();
    std::string typeString();

    std::string serialize();
    static Packet* deserialize(const char* rawData);

  private:
    Packet(int length, PacketType type, int sequenceNumber, long unsigned int timestamp, const char* payload);
    static const char* typeToString(PacketType type);
    static PacketType stringToType(const char* buffer);
};

#endif // __Packet_hpp__