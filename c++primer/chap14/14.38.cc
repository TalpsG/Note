#include <cstddef>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>
class CheckLength {
public:
  CheckLength(std::size_t a) : th(a) {}
  bool operator()(const std::string &str) { return str.size() < th; }

private:
  std::size_t th;
};
int main(int argc, char *argv[]) {
  std::ifstream ifs("words.txt");
  std::string word;
  CheckLength cl(7);
  int less = 0, more = 0;
  while (ifs >> word) {
    if (cl(word)) {
      ++less;
    } else {
      ++more;
    }
  }
  std::cout << less << " " << more << "\n";
  return 0;
}
