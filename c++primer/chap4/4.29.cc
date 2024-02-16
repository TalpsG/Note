#include <iostream>
int main(int argc, char *argv[]) {
  int x[10];
  int *p = x;
  std::cout << sizeof(x) / sizeof(*x) << "\n";
  std::cout << sizeof(p) / sizeof(*p) << "\n";
  return 0;
}
