#include <vector>
#include "Notification.hpp"
#include "lib/utils/string.hpp"

Notification::Notification():
  id(-1),
  sender(""),
  date(""),
  message("")
{}
Notification::Notification(
  int pId, 
  std::string pSender, 
  std::string pDate,
  std::string pMessage
):
  id(pId),
  sender(pSender),
  date(pDate),
  message(pMessage)
{}

void Notification::setDate(long unsigned timestamp) {

}

Notification* Notification::deserialize(std::string raw) {
  int id, length; 
  long unsigned timestamp;
  char senderBuff[45];
  std::vector<std::string> headerAndMessage = splitFirst(raw, "\n\n");
  if (headerAndMessage.size() == 2) {
    int amountRead = sscanf(
      headerAndMessage[0].c_str(), 
      "id:%d\ntimestamp:%lu\nsender:%s\nlength:%d",
      &id, &timestamp, senderBuff, &length);
    
    if (amountRead == 4) {
      std::string message = headerAndMessage[1];
      return new Notification(id, senderBuff, beautifyTime(timestamp), message);
    }
  }

  return NULL;
}