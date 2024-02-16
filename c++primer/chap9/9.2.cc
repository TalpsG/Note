#include <deque>
#include <iostream>
#include <list>
#include <string>
#include <vector>
int main(int argc, char *argv[]) {
  std::list<std::deque<int>> l;
  std::vector<std::string> vec{"talps", "gd"};
  std::vector<std::string>::reference r1 = vec[1];
  r1 = "splat";
  for (const auto &a : vec) {
    std::cout << a << "\n";
  }

  return 0;
}
