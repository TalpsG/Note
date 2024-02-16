#include <map>
#include <string>
#include <vector>
int main(int argc, char *argv[]) {
  std::map<std::string, std::vector<int>> mp;
  std::pair<std::map<std::string, std::vector<int>>::iterator, bool> p =
      mp.insert({"a", std::vector<int>(10, 1)});
  return 0;
}
