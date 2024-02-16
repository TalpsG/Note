#include <iostream>
#include <string>
int main(int argc, char *argv[]) {
  std::string s1{"ab2c3d7RE6"};
  std::string num{"0123456789"};
  auto pos = s1.find_first_of(num);
  std::cout << pos << "\n";
  pos = s1.find_first_not_of(num);
  std::cout << pos << "\n";
  return 0;
}
