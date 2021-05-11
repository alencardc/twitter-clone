#ifndef __ReplicaInfo_hpp__
#define __ReplicaInfo_hpp__

#include <string>
#include <sstream>
#include "lib/utils/string.hpp"

class ReplicaInfo {
  public:
  int id;
  std::string ip;
  int port;

  ReplicaInfo(): id(-1), ip(""), port(-1) {}
  ReplicaInfo(int idParam, std::string ipParam, int portParam):
    id(idParam), ip(ipParam), port(portParam) 
  {}

  bool isValid() {
    return !(id < 0 && port < 0 && ip.size() == 0);
  }

  std::string serialize() {
    std::stringstream stream;
    stream << id << " " << ip << " " << port;
    return stream.str();
  }

  static ReplicaInfo deserialize(const char* rawData) {
    std::vector<std::string> splits = split(rawData, " ");
    ReplicaInfo info;
    if (splits.size() == 3) {
      info.id = atoi(splits[0].c_str());
      info.ip = splits[1];
      info.port = atoi(splits[2].c_str());
    }
    return info;
  }
};

#endif // __ReplicaInfo_hpp__