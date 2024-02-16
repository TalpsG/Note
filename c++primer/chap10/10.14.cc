#include <iostream>
int main(int argc, char *argv[]) {
  int a = 1;
  auto f = [a](int b) { return a + b; };
  std::cout << f(2);
  return 0;
}
