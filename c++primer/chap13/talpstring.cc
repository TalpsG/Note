#include <cstddef>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <memory>
#include <vector>
class talpstring {
public:
  talpstring() : len(0) { p = new char('\0'); }
  talpstring(const char *str) : p(new char(strlen(str) + 1)), len(strlen(str)) {
    strcpy(p, str);
  }
  talpstring(const talpstring &str) : p(new char(str.len + 1)), len(str.len) {
    strcpy(p, str.p);
    std::cout << "copy!" << '\n';
  }
  talpstring &operator=(const talpstring &str) {
    auto np = new char(str.len + 1);
    strcpy(np, str.p);
    delete p;
    p = np;
    len = str.len;
    std::cout << "=\n";
    return *this;
  }

  ~talpstring() { delete p; }

private:
  static std::allocator<talpstring> alloc;
  char *p;
  std::size_t len;
};

std::allocator<talpstring> talpstring::alloc;
int main(int argc, char *argv[]) {
  std::vector<talpstring> vec;
  for (int i = 0; i < 15; i++) {
    char temp[100];
    sprintf(temp, "%d\n", i);
    vec.emplace_back(temp);
  }
  return 0;
}
