#include <iostream>
int a[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
int (&func())[10] { return a; }
int main(int argc, char *argv[]) {
  for (int i = 0; i < 10; ++i) {
    std::cout << func()[i] << "\n";
  }
  return 0;
}
