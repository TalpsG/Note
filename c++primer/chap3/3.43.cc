#include <iostream>
#include <iterator>
int main(int argc, char *argv[]) {
  int ia[3][4] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
  // 范围for
  for (auto &a : ia) {
    for (auto b : a) {
      std::cout << b << " ";
    }
  }
  std::cout << "\n";
  // 下标
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 4; j++) {
      std::cout << ia[i][j] << " ";
    }
  }
  std::cout << "\n";
  // 指针
  for (int(*p1)[4] = std::begin(ia); p1 != std::end(ia); p1++) {
    for (int *p2 = std::begin(*p1); p2 != std::end(*p1); p2++) {
      std::cout << *p2 << " ";
    }
  }
  std::cout << "\n";
  // typedef or using
  using arr = int[4];
  for (arr &p1 : ia) {
    for (int p2 : p1) {
      std::cout << p2 << " ";
    }
  }
  std::cout << "\n";
  // auto
  for (auto p1 = std::begin(ia); p1 != std::end(ia); p1++) {
    for (auto p2 = std::begin(*p1); p2 != std::end(*p1); p2++) {
      std::cout << *p2 << " ";
    }
  }
  std::cout << "\n";
  return 0;
}
