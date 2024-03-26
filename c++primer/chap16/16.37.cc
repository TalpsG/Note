#include <algorithm>
#include <iostream>
#include <memory>
int main(int argc, char *argv[]) {
  int a = 3;
  double b = 4.0;
  int res = std::max<int>(3, 4.0);
  return 0;
}
