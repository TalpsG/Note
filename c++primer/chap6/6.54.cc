#include <iostream>
#include <vector>
int foo(int, int) {
  std::cout << "foo\n";
  return 0;
}
int main(int argc, char *argv[]) {
  std::vector<int (*)(int, int)> vec1;
  vec1.push_back(foo);
  vec1[0](0, 0);
  return 0;
}
