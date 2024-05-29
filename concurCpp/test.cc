#include <chrono>
#include <future>
#include <iostream>
#include <string>
#include <thread>
std::string oops() {
  std::this_thread::sleep_for(std::chrono::seconds(5));
  return std::string("talps");
}
auto func() {
  char buf[] = "talps";
  auto f = std::async(oops);
  std::cout << f.get() << "\n";
}
int main(int argc, char *argv[]) {
  func();
  return 0;
}
