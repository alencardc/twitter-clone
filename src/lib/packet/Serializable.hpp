#ifndef __Serializable_hpp__
#define __Serializable_hpp__

#include <string>

class Serializable {
  public:
    virtual std::string serialize() = 0;
    virtual const Serializable* deserialize(const char* rawData) = 0;
};

#endif // __Serializable_hpp__