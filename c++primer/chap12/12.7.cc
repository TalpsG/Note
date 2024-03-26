#include <iostream>
#include <memory>
#include <vector>
std::shared_ptr<std::vector<int>> alloc() {
  return std::make_shared<std::vector<int>>();
}
void scan(std::shared_ptr<std::vector<int>> p) {
  int temp;
  std::cin >> temp;
  p->push_back(temp);
}
void print(std::shared_ptr<std::vector<int>> p) {
  for (auto i : *p) {
    std::cout << i << '\n';
  }
}
int main(int argc, char *argv[]) {
  auto p = alloc();
  scan(p);
  print(p);
  return 0;
}
