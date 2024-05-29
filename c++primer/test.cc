#include <cstdint>
#include <iostream>
void test(bool b) { std::cout << "bool\n"; }
void test(uint64_t ptr) { std::cout << "ptr\n"; }
int main(int argc, char *argv[]) {
  void *p = NULL;
  void *p2 = nullptr;

  test(p);
  test(p2);

  return 0;
}
