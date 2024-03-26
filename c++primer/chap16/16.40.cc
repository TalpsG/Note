#include <vector>
template <typename It> auto func3(It beg, It end) -> decltype(*beg + 0) {
  return *beg;
}

int main(int argc, char *argv[]) {
  std::vector<int> vec(10, 0);
  int a = func3(vec.begin(), vec.end());
  a = 4;
  return 0;
}
