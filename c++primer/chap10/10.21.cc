#include <iostream>
int main(int argc, char *argv[]) {
  int a = 5;
  auto f = [a]() mutable -> bool {
    if (a == 0)
      return true;
    else {
      --a;
      return false;
    }
  };
  int times = 0;
  while (1) {
    ++times;
    if (f())
      break;
  }
  std::cout << times << "\n";

  return 0;
}
