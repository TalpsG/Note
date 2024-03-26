
#include <iostream>
#include <string>
template <typename T, typename... Args>
void foo(const T &t, const Args &...rest) {
  std::cout << sizeof...(Args) << "\n";
  std::cout << sizeof...(rest) << "\n";
}

int main(int argc, char *argv[]) {
  int i = 0;
  double d = 3.14;
  std::string s = "wwt";
  foo(i, s, 42, d);
  foo(d, s);
  return 0;
}
