#include <iostream>
#include <iterator>
#include <string>
#include <vector>
template <typename T> void print(const T &t) {
  for (auto iter = std::begin(t); iter != std::end(t); ++iter) {
    std::cout << *iter << "\n";
  }
}
int main(int argc, char *argv[]) {
  std::vector<std::string> vec(10, "talps");
  vec[7] = "wwt";
  print(vec);
  return 0;
}
