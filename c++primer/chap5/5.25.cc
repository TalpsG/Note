#include <iostream>
#include <stdexcept>
int main(int argc, char *argv[]) {
  int a, b;
  std::cin >> a >> b;
  try {
    if (b == 0) {
      throw std::runtime_error("divided by 0");
    }
    std::cout << a / b;
  } catch (std::runtime_error err) {
    std::cout << "catch\n";
  }
  return 0;
}
