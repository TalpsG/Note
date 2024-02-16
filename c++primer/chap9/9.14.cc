#include <iostream>
#include <list>
#include <string>
#include <vector>
int main(int argc, char *argv[]) {
  char a[] = "a", b[] = "b", c[] = "c";
  std::list<char *> l1 = {a, b, c};
  std::vector<std::string> vec;
  vec.assign(l1.begin(), l1.end());
  for (auto a : vec) {
    std::cout << a << " ";
  }
  std::cout << "\n";
  return 0;
}
