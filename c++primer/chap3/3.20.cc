#include <iostream>
#include <vector>
int main(int argc, char *argv[]) {
  std::vector<int> vec;
  int n, t;
  std::cin >> n;
  for (int i = 0; i < n; i++) {
    std::cin >> t;
    vec.push_back(t);
  }
  for (std::vector<int>::size_type i = 0; i + 1 < vec.size(); i++) {
    std::cout << vec[i] + vec[i + 1] << "\n";
  }
  return 0;
}
