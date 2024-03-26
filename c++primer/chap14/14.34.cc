#include <algorithm>
#include <iostream>
#include <vector>
class ifelseten {
public:
  void operator()(bool a) {

    if (a) {

      std::cout << "yes\n";
      return;
    }
    std::cout << "no\n";
    return;
  }
};
int main(int argc, char *argv[]) {
  std::vector<int> vec;
  for (int i = 0; i < 10; i++) {
    vec.push_back(i % 2);
  }
  std::for_each(vec.begin(), vec.end(), ifelseten());
  return 0;
}
