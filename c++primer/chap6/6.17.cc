#include <cctype>
#include <iostream>
#include <string>
bool foo(const std::string &str) {
  for (auto c : str) {
    if (isupper(c)) {
      return true;
    }
  }
  return false;
}
void bar(std::string &str) {
  for (auto &c : str) {
    c = tolower(c);
  }
}
int main(int argc, char *argv[]) {
  std::string a = " wtf A";
  if (foo(a)) {
    bar(a);
  }
  std::cout << a << '\n';
  return 0;
}
