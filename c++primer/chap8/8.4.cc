#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

std::vector<std::string> vec;
void readfile(const std::string &filename) {
  std::string temp;
  std::ifstream ifs(filename);
  while (ifs >> temp) {
    vec.push_back(temp);
  }
}
int main(int argc, char *argv[]) {
  readfile("8.1.cc");
  for (auto c : vec) {
    std::cout << c << "\n";
  }
  return 0;
}
