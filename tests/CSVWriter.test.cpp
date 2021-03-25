#include "lib/csv/CSVWriter.hpp"

#include <vector>
#include <string>
#include <stdio.h>


int main() {
  CSVWriter writer = CSVWriter();
  writer.open("data/data5.csv", ';');

  std::vector<std::string> row;
  row.push_back("alencar6");
  row.push_back(std::to_string(5));
  row.push_back("alencar");
  row.push_back("alencar3");

  writer.append(row);

  std::vector<std::string> columns;
  columns.push_back("raphael");
  columns.push_back("filipe");
  // writer.appendToLine(1, "2", columns);

  writer.close();
  return 0;
}