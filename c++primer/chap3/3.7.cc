#include <iostream>
#include <string>
int main(int argc, char *argv[]) {
  std::string t1;
  std::cin >> t1;
  for (char &a : t1) {
    a = 'X';
  }
  std::cout << t1 << "\n";
  return 0;
}
