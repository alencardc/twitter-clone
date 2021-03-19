#ifndef __Packet_hpp__
#define __Packet_hpp__

#include <string>
#include "Serializable.hpp"

enum PacketType {
  DATA,
  COMMAND,
};

class Packet {
  int m_length;
  PacketType m_type;
  int m_sequenceNumber;
  long unsigned int m_timestamp;
  const char* m_payload;

  public:
    Packet(PacketType type, Serializable* data);
    Packet(PacketType type, const char* payload);
    ~Packet();

    std::string serialize();

  private:
    const char* typeToString(PacketType type);
};

#endif // __Packet_hpp__