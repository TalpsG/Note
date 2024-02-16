#include <forward_list>
#include <iostream>
#include <string>
std::forward_list<std::string> fl;
void foo(std::forward_list<std::string> &fl, std::string str1,
         std::string str2) {
  auto prev = fl.before_begin();
  auto cur = fl.begin();
  while (cur != fl.end()) {
    if (*cur == str1) {
      fl.insert_after(cur, str2);
      return;
    } else {
      prev = cur;
      ++cur;
    }
  }
  fl.insert_after(prev, str2);
}
int main(int argc, char *argv[]) {
  fl.insert_after(fl.before_begin(), "456");
  fl.insert_after(fl.before_begin(), "123");
  foo(fl, "123", "talps");
  for (auto &str : fl) {
    std::cout << str << " ";
  }
  return 0;
}
