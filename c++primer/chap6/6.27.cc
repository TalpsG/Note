#include <initializer_list>
#include <iostream>

void foo(std::initializer_list<int> ls) {
  int sum = 0;
  for (auto &a : ls) {
    sum += a;
  }
  std::cout << sum << '\n';
}

int main(int argc, char *argv[]) {
  foo({1, 2, 3, 4, 5, 6});
  return 0;
}
