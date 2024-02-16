#include <deque>
#include <iostream>
#include <string>
int main(int argc, char *argv[]) {
  std::string temp;
  std::deque<std::string> deq;
  while (std::cin >> temp) {
    deq.emplace_back(temp);
  }
  return 0;
}
