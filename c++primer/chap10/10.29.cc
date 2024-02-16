#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>
int main(int argc, char *argv[]) {
  std::vector<std::string> vec;
  std::ifstream ifs("10.29.cc");
  std::istream_iterator<std::string> str_iter(ifs), eof;
  while (str_iter != eof) {
    vec.push_back(*str_iter);
    ++str_iter;
  }
  std::ostream_iterator<std::string> str_o(std::cout, " ");
  for (auto &str : vec) {
    str_o = str;
  }
  return 0;
}
