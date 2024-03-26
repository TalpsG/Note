#include <iostream>
#include <ostream>
#include <string>
#include <vector>
class inputString {
public:
  inputString() : is(std::cin), sep(' ') {}
  std::string operator()() {
    std::string temp;
    if (std::getline(is, temp)) {
      return temp;
    }
    std::cout << " eof or something\n";
    return temp;
  }

private:
  std::istream &is;
  char sep;
};
int main(int argc, char *argv[]) {
  inputString is;
  std::vector<std::string> vec;
  vec.emplace_back(is());
  for (auto &s : vec) {
    std::cout << s << '\n';
  }
  return 0;
}
