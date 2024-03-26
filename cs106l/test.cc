#include <iostream>
#include <thread>
#include <vector>
int times = 0;
void hello1() { std::cout << "t1: " << times << "\n"; }
void hello2() { std::cout << "t2: " << times << "\n"; }
int main(int argc, char *argv[]) {
  ;
  ;

  std::thread t1(hello1), t2(hello2);
  return 0;
}
