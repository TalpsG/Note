#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <strstream>
class Eq {
public:
  bool operator()(const std::string &a, const std::string &b) { return a == b; }
};
int main(int argc, char *argv[]) {
  std::ifstream ifs("words.txt");
  std::string line;
  Eq eq;
  std::string target = "man";
  while (std::getline(ifs, line)) {
    std::istringstream iss(line);
    std::string word;
    while (iss >> word) {
      if (eq(word, target)) {
        std::cout << "woman";
      } else {
        std::cout << word;
      }
      std::cout << " ";
    }
    std::cout << "\n";
  }
  return 0;
}
