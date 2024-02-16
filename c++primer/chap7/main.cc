#include "Window.h"
#include <iostream>

int main(int argc, char *argv[]) {
  Screen myscreen(5, 5, 'c');
  myscreen.move(4, 0).set('#').display(std::cout);
  std::cout << "\n";
  myscreen.display(std::cout);
  std::cout << "\n";
  return 0;
}
