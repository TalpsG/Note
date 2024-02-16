#include <cstdlib>
#include <iostream>
#include <vector>
int main(int argc, char *argv[]) {
  std::vector<int> vec;
  std::vector<int> sum(11, 0);
  for (int i = 0; i < 10; i++) {
    vec.push_back(rand() % 100);
  }
  for (auto i = vec.cbegin(); i != vec.cend(); ++i) {
    ++sum[(*i) / 10];
    std::cout << *i << " ";
  }
  for (auto i = sum.cbegin(); i != sum.cend(); i++) {
    std::cout << *i << " ";
  }
  return 0;
}
