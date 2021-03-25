#include "lib/csv/CSVReader.hpp"

#include <vector>
#include <string>
#include <stdio.h>


int main() {
  CSVReader reader = CSVReader();
  reader.open("data/data.csv", ';');

  std::vector<std::string> row = reader.readLine(3);

  for (auto column : row) {
    printf("%s;", column.c_str());
  }
  printf("\n\n\n");

  std::vector<std::vector<std::string>> rows = reader.readAll();

  for (auto row : rows) {
    for (auto column : row) {
      printf("%s;", column.c_str());
    }
    printf("\n"); 
  }

  printf("\n\n\n");

  row = reader.searchFor(3, "alencar");
  for (auto column : row) {
    printf("%s;", column.c_str());
  }
  printf("\n\n\n");

  reader.close();
  return 0;
}