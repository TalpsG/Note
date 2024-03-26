#include <algorithm>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
class HasPtr {
  friend void swap(HasPtr &, HasPtr &);

public:
  HasPtr(const std::string &s = std::string())
      : ps(new std::string(s)), i(0), use(new std::size_t(1)) {}
  HasPtr(const HasPtr &hp) : ps(hp.ps), i(hp.i), use(hp.use) { ++(*hp.use); }
  HasPtr(HasPtr &&hp) noexcept : ps(hp.ps), i(hp.i), use(hp.use) {
    hp.ps = nullptr;
  }
  HasPtr &operator=(HasPtr hp) {
    swap(*this, hp);
    return *this;
  }
  bool operator<(HasPtr &h) { return *ps < *h.ps; }
  ~HasPtr() {
    if ((--*use) == 0) {
      delete ps;
      delete use;
    }
  }
  std::string &operator*() { return *ps; }

private:
  std::string *ps;
  int i;
  std::size_t *use;
};
inline void swap(HasPtr &h1, HasPtr &h2) {
  using std::swap;
  std::cout << *h1.ps << "hp swap"
            << " " << *h2.ps << "\n";
  swap(h1.ps, h2.ps);
  swap(h1.i, h2.i);
}
int main(int argc, char *argv[]) {
  std::vector<HasPtr> vec;
  std::string temp;
  std::ifstream ifs("words.txt");
  while (ifs >> temp) {
    vec.push_back({temp});
  }
  std::sort(vec.begin(), vec.end());
  for (auto i : vec) {
    std::cout << *i << "\n";
  }
  return 0;
}
