#ifndef __CSVWriter_hpp__
#define __CSVWriter_hpp__

#include <variant>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>

class CSVWriter {
  std::fstream m_file;
  std::string m_filename;
  char m_delimiter;

  public:
    CSVWriter();

    bool open(std::string filename, char delimiter = ',');

    void close();

    bool canWrite();

    bool append(std::vector<std::string> row);
    
    bool appendToLine(
      int columnNumber, 
      std::string seachFor, 
      std::vector<std::string> newColumns
    );

  private:
    std::string formatLine(std::vector<std::string> line);
};

#endif // __CSVWriter_hpp__