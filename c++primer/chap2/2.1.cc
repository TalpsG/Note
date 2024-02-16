#include <iostream>

int main(int argc, char *argv[]) {
  std::cout << sizeof(int) << " " << sizeof(long) << " " << sizeof(long long)
            << " " << sizeof(float) << " " << sizeof(double) << " "
            << sizeof(long double) << "\n";
  return 0;
}
