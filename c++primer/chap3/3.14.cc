#include <iostream>
#include <vector>
int main(int argc, char *argv[]) {
  int n;
  std::vector<int> vec;
  std::cin >> n;
  for (int i = 0; i < n; i++) {
    int t;
    std::cin >> t;
    vec.push_back(t);
  }
  for (auto i : vec) {
    std::cout << "\n" << i;
  }
  return 0;
}
