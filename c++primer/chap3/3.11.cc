#include <iostream>
#include <string>
int main(int argc, char *argv[]) {
  const std::string t1 = "talps";
  for (auto &a : t1) {
    std::cout << a;
  }

  return 0;
}
