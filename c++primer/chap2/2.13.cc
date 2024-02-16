#include <iostream>
int i = 2;
int main(int argc, char *argv[]) {
  int i = 100;
  int j = i;
  std::cout << j << "\n";
  j = ::i;
  std::cout << j << "\n";
  return 0;
}
