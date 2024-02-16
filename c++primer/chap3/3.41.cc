#include <iostream>
#include <vector>
int main(int argc, char *argv[]) {
  int a[] = {1, 2, 3, 4, 5};
  std::vector<int> vec(a, a + 4);
  for (auto i : vec) {
    std::cout << i << " ";
  }
  return 0;
}
