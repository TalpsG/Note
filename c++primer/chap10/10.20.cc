#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
int main(int argc, char *argv[]) {
  std::string temp;
  std::vector<std::string> vec;
  while (std::cin >> temp) {
    vec.push_back(temp);
  }
  for (auto &str : vec) {
    std::cout << str << " ";
  }
  std::cout << '\n';
  auto cnt =
      std::count_if(vec.cbegin(), vec.cend(),
                    [](const std::string &s) -> bool { return s.size() > 6; });
  std::cout << cnt << "\n";

  return 0;
}
