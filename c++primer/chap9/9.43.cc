#include <iostream>
#include <string>
void foo(std::string &s, std::string old, std::string n) {
  for (auto i = s.begin(); i != s.end(); ++i) {
    if (i + old.size() > s.end()) {
      return;
    }
    if (s.substr(i - s.begin(), old.size()) == old) {
      s.replace(i, i + old.size(), n);
      i = i + old.size();
    }
  }
}
int main(int argc, char *argv[]) {
  std::string s{"123tho"};
  foo(s, "tho", "though");
  std::cout << s;
  std::string s1{"123"};
  s1.insert(s1.end(), 'c');
  return 0;
}
