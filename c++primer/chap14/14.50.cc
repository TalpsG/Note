#include <iostream>
struct LD {
  LD(double = 0) { std::cout << "double ctor\n"; };
  operator double() {
    std::cout << "double convert\n";
    return 1.1;
  }
  operator float() {
    std::cout << "float convert\n";
    return 2.2;
  }
};
int main(int argc, char *argv[]) {
  LD ld;
  float b = ld;
  return 0;
}
