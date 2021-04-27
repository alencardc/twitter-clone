#ifndef __HostAddress_hpp__
#define __HostAddress_hpp__

#include <string>
#include <sstream>
#include "lib/packet/Serializable.hpp"

class HostAddress {
  public:
  std::string ip;
  int port;

  HostAddress() = default;
  HostAddress(std::string ipParam, int portParam):
    ip(ipParam), port(portParam) 
  {}

  std::string serialize() {
    std::stringstream stream;
    stream << ip << " " << port;
    return stream.str();
  }

  static HostAddress deserialize(const char* rawData) {
    std::stringstream stream;
    stream << rawData;
    char strIp[16];
    stream.getline(strIp, 15, ' ');
    HostAddress addr;
    addr.ip = strIp;
    stream >> addr.port;
    return addr;
  }
};

#endif // __HostAddress_hpp__