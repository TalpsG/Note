#include <cstdlib>
#include <iostream>
#include <vector>
int main(int argc, char *argv[]) {
  std::vector<int> vec{1, 2, 3, 4, 5, 6, 7, 8};
  auto begin = vec.begin();
  while (begin != vec.end()) {
    if (*begin % 2)
      begin = vec.insert(begin, rand() % 10 + 10);
    ++begin;
  }
  for (auto i : vec) {
    std::cout << i << " ";
  }
  return 0;
}
