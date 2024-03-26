#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
template <typename T> T abs(T a) { return -a; }
template <typename T> int find_cnt(std::vector<T> &vec, const T &target) {
  int cnt = 0;
  for (auto iter = vec.cbegin(); iter != vec.cend(); ++iter) {
    if (*iter == target)
      ++cnt;
  }
  return cnt;
}
template <> int find_cnt(std::vector<double> &vec, const double &target) {
  int cnt = 0;
  for (auto iter = vec.cbegin(); iter != vec.cend(); ++iter) {
    if (abs<double>(*iter - target) < 0.000001)
      ++cnt;
  }
  return cnt;
}
int main(int argc, char *argv[]) {
  std::vector<double> vd(10, 0);
  vd[2] = vd[4] = 3.14;
  std::vector<int> vi(10, 0);
  vi[2] = vi[4] = 3;
  std::vector<std::string> vs(10);
  std::string s = "wwt";
  vs[2] = s;
  vs[4] = s;
  std::cout << "double : " << find_cnt(vd, 3.14) << "\n";
  std::cout << "int    : " << find_cnt(vi, 3) << "\n";
  std::cout << "string : " << find_cnt(vs, s) << "\n";

  return 0;
}
