#ifndef __global_config_hpp__
#define __global_config_hpp__

#include <vector>
#include <string>

namespace GlobalConfig {
  struct Address {
    std::string ip;
    int port;
  };

  static const std::vector<Address> FRONTEND_ADDRESSES = {
    {"0.0.0.0", 5555},
    {"0.0.0.0", 6666},
    {"0.0.0.0", 7777},
    {"0.0.0.0", 8888}
  };

  static const Address SERVER_ADDR = {"0.0.0.0", 8080};
}

#endif // __global_config_hpp__