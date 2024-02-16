#include <iostream>
#include <string>
int main(int argc, char *argv[]) {
  std::string prev, now;
  if (!(std::cin >> prev)) {
    std::cout << "there is only one word\n";
  }
  while (std::cin >> now) {
    if (now == prev) {
      std::cout << now << '\n';
      return 0;
    }
    prev = now;
  }
  std::cout << "no same two words\n";

  return 0;
}
