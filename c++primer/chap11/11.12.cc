#include <iostream>
#include <string>
#include <utility>
#include <vector>
int main(int argc, char *argv[]) {
  std::pair<std::string, int> p1{"hello ", 1};
  std::vector<std::pair<std::string, int>> vec{p1};
  for (auto &e : vec) {
    std::cout << e.first << " " << e.second << '\n';
  }
  return 0;
}
