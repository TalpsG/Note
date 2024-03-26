#include <iostream>
#include <utility>
void swap(int &a, int &b) {
  int temp = a;
  a = b;
  b = temp;
}
template <typename F, typename T1, typename T2>
void flip(F f, T1 &&t1, T2 &&t2) {
  swap(std::forward<T1>(t1), std::forward<T2>(t2));
}
int main(int argc, char *argv[]) {
  int a = 1, b = 2;
  flip(swap, a, b);
  std::cout << "a: " << a << " b: " << b << "\n";
  return 0;
}
