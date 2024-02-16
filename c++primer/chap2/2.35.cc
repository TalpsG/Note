#include <iostream>
using namespace std;
int main(int argc, char *argv[]) {
  const int i = 42;
  auto j = i;
  const auto &k = i;
  auto *p = &i;
  cout << "j:" << typeid(j).name() << " :int "
       << "\n";
  cout << "k:" << typeid(k).name() << " :const int"
       << "\n";
  cout << "p:" << typeid(p).name() << " :const int *"
       << "\n";
  return 0;
}
