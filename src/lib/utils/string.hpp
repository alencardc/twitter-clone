#ifndef __Utils_String_hpp__
#define __Utils_String_hpp__
#include <string>
#include <vector>

bool hasPrefix(std::string origin, std::string prefix);
std::string removePrefix(std::string origin, std::string prefix);
std::string removeNewLine(std::string origin, bool removeAll);

std::vector<std::string> splitFirst(const std::string& str, const std::string& delimiter);
std::vector<std::string> split(const std::string& str, const std::string& delimiter);
std::vector<std::string> splitByLength(long unsigned length, std::string s);
std::string beautifyTime(long unsigned timestamp);

#endif // __Utils_String_hpp__