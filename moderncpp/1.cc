#include <functional>
#include <future>
#include <iostream>
#include <vector>

auto operator|(std::vector<int> &v, std::function<void(int &)> f) {
  for (auto &i : v) {
    f(i);
  }
  return v;
}
auto operator|(std::vector<int> &&v, std::function<void(const int &)> f) {
  for (auto &i : v) {
    f(i);
  }
  return v;
}
int main() {

  std::vector v{1, 2, 3};
  std::function f{[](const int &i) { std::cout << i << ' '; }};
  auto f2 = [](int &i) { i *= i; };
  v | f2 | f;
}
