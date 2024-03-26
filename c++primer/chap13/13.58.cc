#include <iostream>
class Foo {
public:
  Foo &sorted() && {
    std::cout << "rvalue sorted\n";
    return *this;
  }
  Foo sorted() const & {
    std::cout << "lvalue sorted\n";
    return Foo(*this).sorted();
  }
};
int main(int argc, char *argv[]) {
  Foo f;
  f.sorted();
  return 0;
}
