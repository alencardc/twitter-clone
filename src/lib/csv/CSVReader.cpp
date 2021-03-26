#include <iostream>
#include <sstream>
#include "CSVReader.hpp"



CSVReader::CSVReader(): m_filename(""), m_delimiter(',') {}


bool CSVReader::open(std::string filename, char delimiter = ',') {
  m_file.open(filename, std::fstream::in);

  m_delimiter = delimiter;
  
  return m_file.is_open();
}

void CSVReader::close() {
  if (m_file.is_open()) {
    m_file.close();
  }
}

bool CSVReader::canRead() {
  return m_file.eof() == false && m_file.fail() == false && m_file.bad() == false;
}

std::vector<std::string> CSVReader::readLine(int lineNumber) {
  std::vector<std::string> row;
  if (m_file.is_open()) {
    m_file.seekg(0);
    std::string line;

    int i = 0;
    while (canRead() == true && i < lineNumber) {
      std::getline(m_file, line);
      i++;
    }

    if (canRead() == true) {
      std::getline(m_file, line);
      std::stringstream rowStream(line);
      std::string column;
      while (std::getline(rowStream, column, m_delimiter)) {
        row.push_back(column);
      }
    }
  }

  return row;
}

void CSVReader::goToBegin() {
  if (m_file.is_open()) {
    m_file.seekg(0);
  }
}

std::vector<std::string> CSVReader::searchFor(long unsigned int columnNumber, std::string content) {
  std::vector<std::string> row;

  if (m_file.is_open()) {
    m_file.seekg(0);
    std::string line;
    std::string testColumn;

    do {
      row.clear();
      std::getline(m_file, line);
      std::stringstream rowStream(line);
      std::string column;
      while (std::getline(rowStream, column, m_delimiter)) {
        row.push_back(column);
      }

      if (row.size() > columnNumber && row[columnNumber] == content) {
        return row;
      }
    } while (canRead() && content != testColumn);
  }

  row.clear();
  return row;
}

std::vector<std::vector<std::string>> CSVReader::readAll() {
  std::vector<std::vector<std::string>> rows;
  if (m_file.is_open()) {
    m_file.seekg(0);

    std::string line;
    while (canRead() == true) {
      std::getline(m_file, line);

      std::stringstream rowStream(line);
      std::vector<std::string> row;
      std::string column;
      while (std::getline(rowStream, column, m_delimiter)) {
        row.push_back(column);
      }
      
      rows.push_back(row);
    }
  }

  return rows;
}
