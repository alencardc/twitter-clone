#include <sstream>
#include "CSVWriter.hpp"

CSVWriter::CSVWriter(): m_filename(""), m_delimiter(',') {}

bool CSVWriter::open(std::string filename, char delimiter) {
  m_file.open(filename, std::fstream::in | std::fstream::out);

  m_delimiter = delimiter;
  
  return m_file.is_open();
}

void CSVWriter::close() {
  if (m_file.is_open()) {
    m_file.close();
  }
}

bool CSVWriter::canWrite() {
  return m_file.eof() == false && m_file.fail() == false && m_file.bad() == false;
}

bool CSVWriter::append(std::vector<std::string> row) {
  if (m_file.is_open() && row.size() > 0) {
    m_file.seekp(0, std::ios::end);
    std::string formattedRow = "\n" + formatLine(row);
    m_file << formattedRow;
    return true;
  }
  return false;
}

bool CSVWriter::appendToLine(
  int columnNumber, 
  std::string seachFor, 
  std::vector<std::string> newColumns
) {
  std::vector<std::vector<std::string>> rows;
  if (m_file.is_open()) {
    m_file.seekg(0);

    std::string line;
    while (m_file.eof() == false) {
      std::getline(m_file, line);

      std::stringstream rowStream(line);
      std::vector<std::string> row;
      std::string column;
      while (std::getline(rowStream, column, m_delimiter)) {
        row.push_back(column);
      }
      rows.push_back(row);
    }

    for (auto iterator = rows.begin() ; iterator != rows.end(); ++iterator) {
      if ((*iterator).size() > columnNumber && (*iterator)[columnNumber] == seachFor) {
        for (auto col : newColumns) {
          (*iterator).push_back(col);
        }
      }
    }

    m_file.seekg(0);
    if (rows[0].size() > 0) {
      m_file << formatLine(rows[0]);
      for (int i = 1; i < rows.size(); i++) {
        m_file << "\n" + formatLine(rows[i]);
      }
      return true;
    }
  }

  return false;
}

std::string CSVWriter::formatLine(std::vector<std::string> line) {
  std::string formattedRow = line[0];
  for (int i = 1; i < line.size(); i++) {
    formattedRow += m_delimiter + line[i];
  }
  return formattedRow;
}