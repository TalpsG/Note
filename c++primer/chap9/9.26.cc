#include <iostream>
#include <iterator>
#include <list>
#include <vector>
int main(int argc, char *argv[]) {
  int a[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
  std::vector<int> vec(std::begin(a), std::end(a));
  std::list<int> ls(std::begin(a), std::end(a));
  auto i = vec.begin();
  while (i != vec.end()) {
    if (*i % 2 == 0) {
      i = vec.erase(i);
    } else {
      ++i;
    }
  }
  for (auto i : vec) {
    std::cout << i << " ";
  }
  std::cout << "\n";
  auto j = ls.begin();
  while (j != ls.end()) {
    if (*j % 2 == 0) {
      j = ls.erase(j);
    } else {
      ++j;
    }
  }
  for (auto i : ls) {
    std::cout << i << " ";
  }
  std::cout << "\n";

  return 0;
}
