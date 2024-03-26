#include <iostream>
#include <string>
class Foo {
public:
  Foo(const std::string &str) : name(str) {}
  operator std::string() { return "Foo:" + name; }
  std::string name;
};
int main(int argc, char *argv[]) {
  Foo f("talps");
  std::cout << static_cast<std::string>(f) << "\n";
  return 0;
}
