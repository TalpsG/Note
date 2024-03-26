
#include <iostream>
#include <ostream>
using namespace std;
template <typename T> ostream &print(ostream &os, const T &t) {
  return os << t;
}
template <typename T, typename... Args>
ostream &print(ostream &os, const T &t, const Args &...rest) {
  os << t << ",";
  return print(os, rest...);
}
int main(int argc, char *argv[]) {
  print(cout, "1234", 2, "wwt");
  return 0;
}
