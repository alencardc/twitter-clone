#include <algorithm>
#include <time.h>
#include "string.hpp"

bool hasPrefix(std::string origin, std::string prefix) {
  return origin.rfind(prefix, 0) == 0;
}

std::string removePrefix(std::string origin, std::string prefix) {
  if (origin.rfind(prefix, 0) == 0) {
    origin.erase(0, prefix.length());
  }
  return origin;
}

std::string removeNewLine(std::string origin, bool removeAll) {
  if (removeAll) {
    origin.erase(std::remove(origin.begin(), origin.end(), '\n'), origin.end());
  } else {
    if (!origin.empty() && origin[origin.length()-1] == '\n') {
      origin.erase(origin.length()-1);
    }
  }
  return origin;
}

std::vector<std::string> splitFirst(const std::string& str, const std::string& delimiter) {
  std::vector<std::string> parts = std::vector<std::string>();
  std::string strCopy = str;

  std::string first = strCopy.substr(0, strCopy.find(delimiter));
  std::string second = strCopy.erase(0, strCopy.find(delimiter) + delimiter.length());
  parts.push_back(first);
  parts.push_back(second);
  return parts;
}

std::vector<std::string> split(const std::string& str, const std::string& delimiter) {
  std::vector<std::string> parts = std::vector<std::string>();
  std::string strCopy = str;

  size_t pos;
  std::string part;
  while((pos = strCopy.find(delimiter)) != std::string::npos) {
    part = strCopy.substr(0, pos);
    parts.push_back(part);
    strCopy.erase(0, pos + delimiter.length());
  }
  parts.push_back(strCopy);
  return parts;
}

std::vector<std::string> splitByLength(long unsigned length, std::string s) {
  std::string buff = s;
  std::vector<std::string> strings;
  strings.push_back(buff.substr(0, length));
  while (buff.size() > length) {
    buff = buff.substr(length);
    strings.push_back(buff.substr(0, length));
  }
  return strings;
}

std::string beautifyTime(long unsigned timestamp) {
  time_t time = timestamp;
  struct tm* timeinfo = localtime(&time);
  char buffer[35];
  strftime(buffer, sizeof(buffer), "%d %h %Y at %T", timeinfo);
  return std::string(buffer);
}