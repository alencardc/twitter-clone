#include <stdio.h>
#include <string>
#include <sstream>
#include "lib/packet/Packet.hpp"
#include "lib/packet/Serializable.hpp"

class SomePacket: public Serializable {
  int myInt;
  std::string myString;

  public:
    SomePacket(int number, std::string text): myInt(number), myString(text) {}
    ~SomePacket() {};

    std::string serialize() {
      std::stringstream serialized;
      serialized << "myInt:" << myInt << "\nmyString:" << myString << "\n";
      std::string text = serialized.str();
      return text.c_str();
    }

    SomePacket* deserialize(const char* rawData) {
      int number;
      char buffer[256];
      int amountRead = sscanf(rawData, "myInt:%d\nmyString:%s", &number, buffer);
      if (amountRead == 2) {
        return new SomePacket(number, std::string(buffer));
      }
      return NULL;
    }
};

int main() {
  
  SomePacket* data = new SomePacket(42, "That's it.");
  printf("== Data:\n%s\n\n", data->serialize().c_str());

  Packet* packet = new Packet(DATA, data);

  printf("== Payload:\n%s\n\n", packet->serialize().c_str());

  delete data;
  delete packet;


  return 0;
}