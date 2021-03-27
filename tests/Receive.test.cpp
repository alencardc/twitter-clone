#include <string>
#include <vector>
#include <string.h>
#include <stdio.h>
#include "lib/packet/Packet.hpp"

#define RECV_BUFFER_SIZE 400

std::string m_buffer;

std::vector<std::string> socket;
int count = 0;
int recv(char* buffer) {
  if (count < socket.size()) {
    memcpy(buffer, socket[count].c_str(), socket[count].size());
    int size=socket[count].size();
    count++;
    return size;
  }
  return 0;
}


Packet* breakPayload(Packet* packet) {
  std::string payload = packet->payload();
  //printf("=======\n");
  //printf("l=%d s=%lu\n",packet->length(), payload.size());
  //printf("%s\n",packet->serialize().c_str());
  if (packet->length() < payload.size()) {
    //printf("eh menor\n");
    //printf("exedente: %s\n",payload.substr(packet->length(), std::string::npos).c_str());
    m_buffer.append(payload.substr(packet->length(), std::string::npos));
    payload.erase(packet->length(), std::string::npos);
    delete packet->m_payload;
    packet->m_payload = strdup(payload.c_str());
  }
  return packet;
}

Packet* receive() {
  char buffer[RECV_BUFFER_SIZE];

  if (m_buffer.empty() == false) {
    Packet *packet = Packet::deserialize(m_buffer.c_str());
    printf("Buffer not empty\n");
    if (packet->type() != NO_TYPE) {
      printf("No Error\n");
      //m_buffer.erase(0, packet->serialize().size());
      m_buffer.erase(0, m_buffer.size());
      packet = breakPayload(packet);
      return packet;
    } else if(m_buffer.find("type:") != 0) {
      printf("Limpou\n");
      m_buffer.clear();
    }
  }
  //printf("Buffer empty\n");
  int length = recv(buffer);
  //printf(">>>> Recv data: %s\n<<<<<<<\n", buffer);
  if (length > 0) {
    printf(">>>> Recv length: %d\n", length);
    buffer[length] = '\0';
    //printf("####\n%s\n#####",buffer);
    Packet *packet = Packet::deserialize(buffer);
    //printf("   %d   ####\n", packet->length());
    if (packet->type() != NO_TYPE) {
      packet = breakPayload(packet);
      //m_buffer.clear();
    } else if (m_buffer.empty() == false) {
      m_buffer.append(buffer);
      packet = Packet::deserialize(m_buffer.c_str());
      //printf("===Content:%s\n", m_buffer.c_str());
      if (packet->type() != NO_TYPE) {
        m_buffer.erase(0, m_buffer.size());
        packet = breakPayload(packet);
      } else {
        m_buffer.clear();
      }
    }
    return packet;
  }
  m_buffer.clear();
  return NULL;
}

int main() {
  // socket.push_back("type:DATA\ntimestamp:1616806642\nsequenceNumber:15417\nlength:59\n\nid:5\ntimestamp:1616806632\nsender:alencar\nlength:8\n\nAlencar1");
  // socket.push_back("type:DATA\ntimestamp:1616806642\nsequenceNumber:15417\nlength:59\n\nid:5\ntimestamp:1616806632\nsender:alencar\nlength:8\n\nAlencar2type:DATA\ntimestamp:1616806642\nsequenceNumber:15417\nlength:59\n\nid:5\ntimestamp:1616806632\nsender:alencar\nlength:8\n\nAlencar3type:DATA\ntimestamp:1616806642\nsequenceNumber:15417\nlength:59\n\nid:5\ntimestamp:1616806632\nsender:alencar\nlength:8\n\nAlencar4");
  // socket.push_back("type:DATA\ntimestamp:1616806642\nsequenceNumber:15417\nlength:59\n\nid:5\ntimestamp:1616806632\nsender:alencar\nlength:8\n\nAlencar5");
  
  // socket.push_back("type:DATA\ntimestamp:1616806642\nsequenceNumber:15417\nlength:59\n\nid:5\ntimestamp:1616806632\nsender:alencar\nlength:8\n\nAlencar1");
  // socket.push_back("type:DATA\ntimestamp:1616806642\nsequenceNumber:15417\nlength:59\n\nid:5\ntimestamp:1616806632\nsender:alencar\nlength:8\n\nAlencar2type:DATA\ntimestamp:1616806642\nsequenceNumber:15417\nlength:59\n\nid:5\ntimestamp:1616806632\nsender:alencar\nlength:8\n\nAlencar3type:DATA\ntimestamp:1616806642\nsequenceNumber");
  // socket.push_back(":15417\nlength:59\n\nid:5\ntimestamp:1616806632\nsender:alencar\nlength:8\n\nAlencar4type:DATA\ntimestamp:1616806642\nsequenceNumber:15417\nlength:59\n\nid:5\ntimestamp:1616806632\nsender:alencar\nlength:8\n\nAlencar5");

  // socket.push_back("type:DATA\ntimestamp:1616806642\nsequenceNumber:15417\nlength:59\n\nid:5\ntimestamp:1616806632\nsender:alencar\nlength:8\n\nAlencar1");
  // socket.push_back("type:DATA\ntimestamp:1616806642\nsequenceNumber:15417\nlength:59\n\nid:5\ntimestamp:1616806632\nsender:alencar\nlength:8\n\nAlencar2type:DATA\ntimestamp:1616806642\nsequenceNumber:15417\nlength:59\n\nid:5\ntimestamp:1616806632\nsender:alencar\nlength:8\n\nAlencar3type:DATA\ntimestamp:1616806642\nsequenceNumber");
  // socket.push_back("type:DATA\ntimestamp:1616806642\nsequenceNumber:15417\nlength:59\n\nid:5\ntimestamp:1616806632\nsender:alencar\nlength:8\n\nAlencar5");

  socket.push_back("type:DATA\ntimestamp:1616806642\nsequenceNumber:15417\nlength:59\n\nid:5\ntimestamp:1616806632\nsender:alencar\nlength:8\n\nAlencar1");
  socket.push_back("type:DATA\ntimestamp:1616806642\nsequenceNumber:15417\nlength:59\n\nid:5\ntimestamp:1616806632\nsender:alencar\nlength:8\n\nAlencar2type:DATA\ntimestamp:1616806642\nsequenceNumber:15417\nlength:59\n\nid:5\ntimestamp:1616806632\nsender:alencar\nlength:8\n\nAlencar3safasfasf");
  socket.push_back("type:DATA\ntimestamp:1616806642\nsequenceNumber:15417\nlength:59\n\nid:5\ntimestamp:1616806632\nsender:alencar\nlength:8\n\nAlencar5");

  Packet* packet;
  int i = 0;
  while ((packet = receive()) != NULL && i < 15) {
    i++;
    //printf("========\nReceived: %s\n========\n", packet->serialize().c_str());
    printf("========\nReceived: %s\n========%s==\n", packet->payload(), m_buffer.c_str());
  }

  printf("#######\nFinal content on: %s\n######\n", m_buffer.c_str());
  return 0;
}

//type:DATA\ntimestamp:1616806642\nsequenceNumber:15417\nlength:64\n\nid:5\ntimestamp:1616806632\nsender:alencar\nlength:12\n\nAKKLASFJSKAFtype:DATA\ntimestamp:1616806642\nsequenceNumber:15418\nlength:64\n\nid:6\ntimestamp:1616806635\nsender:alencar\nlength:12\n\nJSFJBASKFKSA