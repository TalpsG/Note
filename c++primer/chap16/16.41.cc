#include <type_traits>
template <typename T> auto sum(const T &t1, const T &t2) -> decltype(t1 + t2) {
  return t1 + t2;
}

int main(int argc, char *argv[]) {
  int a = 1, b = 2;
  int p = sum(a, b);
  return 0;
}
