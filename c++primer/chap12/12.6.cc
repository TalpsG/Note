#include <iostream>
#include <vector>
auto alloc() -> std::vector<int> * { return new std::vector<int>; }
void scan(std::vector<int> *p) {
  int temp;
  std::cin >> temp;
  p->push_back(temp);
}
void print(std::vector<int> *p) {
  for (auto i : *p) {
    std::cout << i << '\n';
  }
}

int main(int argc, char *argv[]) {
  auto p = alloc();
  scan(p);
  print(p);
  delete p;
  return 0;
}
