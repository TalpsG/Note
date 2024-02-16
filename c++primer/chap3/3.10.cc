#include <cctype>
#include <iostream>
#include <string>
int main(int argc, char *argv[]) {
  std::string t1;
  std::cin >> t1;
  for (char a : t1) {
    if (isalpha(a)) {
      std::cout << a;
    }
  }
  std::cout << "\n";
  return 0;
}
