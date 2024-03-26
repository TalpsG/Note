#include <iostream>
#include <string>
#include <vector>
template <typename T> void print(const T &t) {
  for (typename T::size_type i = 0; i < t.size(); i++) {
    std::cout << i << " : " << t[i] << "\n";
  }
}
int main(int argc, char *argv[]) {
  std::vector<std::string> vec(10, "talps");
  vec[7] = "wwt";
  print(vec);
  return 0;
}
