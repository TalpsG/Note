#include <iostream>
#include <thread>

int main(int argc, char *argv[]) {
  int a = 0;
  std::thread t{[&] { ++a; }};
  t.detach();
  return 0;
}
