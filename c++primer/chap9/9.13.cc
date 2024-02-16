#include <algorithm>
#include <iostream>
#include <list>
#include <vector>
int main(int argc, char *argv[]) {
  std::list<int> ls;
  for (int i = 0; i < 10; i++) {
    ls.push_back(i);
  }
  std::vector<double> vec(ls.begin(), ls.end());
  for (auto &i : vec) {
    std::cout << i << " ";
  }
  std::cout << "\n";
  return 0;
}
