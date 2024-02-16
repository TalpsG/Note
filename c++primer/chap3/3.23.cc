#include <cstdlib>
#include <iostream>
#include <vector>
int main(int argc, char *argv[]) {
  std::vector<int> vec;
  for (int i = 0; i < 10; i++) {
    vec.push_back(rand() % 10);
  }
  for (auto i = vec.cbegin(); i != vec.cend(); ++i) {
    std::cout << *i << "\n";
  }
  for (auto i = vec.begin(); i != vec.end(); ++i) {
    *i = 2 * (*i);
  }
  for (auto i = vec.cbegin(); i != vec.cend(); ++i) {
    std::cout << *i << "\n";
  }
  return 0;
}
