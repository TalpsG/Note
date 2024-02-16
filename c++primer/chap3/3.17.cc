#include <cctype>
#include <iostream>
#include <string>
#include <vector>
int main(int argc, char *argv[]) {
  std::vector<std::string> vec;
  std::string temp;
  while (1) {
    std::cin >> temp;
    if (temp == "0")
      break;
    vec.push_back(temp);
  }

  for (auto &s : vec) {
    for (auto &c : s) {
      c = std::toupper(c);
    }
  }
  std::cout << '\n';

  for (auto &s : vec) {
    std::cout << s << "\n";
  }

  return 0;
}
