#include <iostream>
#include <string>
int main(int argc, char *argv[]) {
  std::string s = "word";
  std::string p1 = s + (s[s.size() - 1] == 's' ? "" : "s");
  std::cout << p1 << '\n';
  return 0;
}
