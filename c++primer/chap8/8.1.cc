#include <iostream>
#include <istream>
#include <sstream>
#include <string>

std::istream &foo(std::istream &is) {
  std::string temp;
  while (is >> temp) {
    std::cout << temp;
  }
  is.clear();
  std::cout << is.get() << "\n";
  return is;
}
int main(int argc, char *argv[]) {
  std::string s{"where there is a shell, there is a way\n"};
  std::stringstream strm(s);
  foo(strm);

  return 0;
}
