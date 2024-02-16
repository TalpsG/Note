#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

int main(int argc, char *argv[]) {
  std::fstream fs("8.1.cc");
  std::vector<std::string> vec;
  std::string temp;
  while (std::getline(fs, temp)) {
    vec.push_back(temp);
    std::string word;
    std::stringstream st(temp);
    while (st >> word) {
      std::cout << word << " ";
    }
    std::cout << '\n';
  }
  std::string str;
  std::ostringstream ostrm(str);
  ostrm << "talps" << std::flush;
  std::cout << "string:" << str << '\n';
  std::cout << "ostrm:" << ostrm.str() << '\n';

  return 0;
}
