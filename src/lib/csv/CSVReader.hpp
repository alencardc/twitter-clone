#ifndef __CSVReader_hpp__
#define __CSVReader_hpp__

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>

class CSVReader {
  std::fstream m_file;
  std::string m_filename;
  char m_delimiter;

  public:
    CSVReader();

    bool open(std::string filename, char delimiter);

    void close();

    bool canRead();

    std::vector<std::string> readLine(int lineNumber);
    std::vector<std::string> searchFor(int columnNumber, std::string content);

    std::vector<std::vector<std::string>> readAll();
};

#endif // __CSVReader_hpp__