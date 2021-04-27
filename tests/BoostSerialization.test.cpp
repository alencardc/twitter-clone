#include <boost/serialization/serialization.hpp>
#include <boost/serialization/unordered_map.hpp>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <iostream>
#include <sstream>

#include "server/notification/Notification.hpp"

template <typename Archive>
void serialize(Archive &ar, Notification &a, const unsigned int version) {
  ar & a.id;
  ar & a.message;
  ar & a.pendingCount;
  ar & a.timestamp;
  ar & a.username;
}

int main() {
  Notification n;
  n.id = 1;
  n.message = "Hello, world!!";
  n.pendingCount = 3;
  n.timestamp = 213;
  n.username = "@alencar";

  std::unordered_map<std::string, Notification> map, map2;
  map.emplace(n.username, n);
  n.username = "@filipe";
  map.emplace(n.username, n);
  n.username = "@raphael";
  map.emplace(n.username, n);

  std::stringstream ss;
  boost::archive::text_oarchive oa{ss};
  oa << map;
  std::cout << ss.str();

  boost::archive::text_iarchive ia{ss};
  ia >> map2;
  std::cout << std::endl << map2["@alencar"].message << std::endl;

  std::string ip = "192.168.1.1"; int port = 3333;
  std::stringstream stream;
  stream << ip << " " << port;
  std::cout << std::endl << stream.str() << std::endl;
  std::string ip2; int port2;
  char strIp[16];
  stream.getline(strIp, 16, ' ');
  ip2 = strIp;
  stream >> port2;
  std::cout << std::endl << ip2 << " - " << port2 << std::endl;
  return 0;
}