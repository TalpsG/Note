#include <iostream>
struct X {
  X() {
    std::cout << "X()"
              << "\n";
  }
  X(const X &) { std::cout << "X(const X&)" << '\n'; }
  X &operator=(const X &x) {
    std::cout << " operator=" << '\n';
    return *this;
  }
  ~X() { std::cout << "~X()" << '\n'; }
};
void foo(struct X x) {}
struct X bar() {
  struct X x;
  return x;
}
int main(int argc, char *argv[]) {
  struct X x;
  struct X y(x);
  std::cout << "func" << '\n';
  foo(x);
  std::cout << "func over" << '\n';
  bar();
  return 0;
}
