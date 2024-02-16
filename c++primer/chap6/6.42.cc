#include <cstddef>
#include <iostream>
#include <string>
std::string make_plural(size_t ctr, const std::string &word,
                        const std::string &ending = "s") {
  return (ctr > 1) ? word + ending : word;
}
int main(int argc, char *argv[]) {
  std::string a = " talps";
  a = make_plural(2, a, "abc");
  std::cout << a << "\n";
  return 0;
}
