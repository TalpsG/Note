#include <iostream>
#include <map>
#include <string>
int main(int argc, char *argv[]) {
  std::multimap<std::string, int> mp;
  mp.insert({"ssh", 1});
  auto p = mp.find("wwt");
  if (p == mp.end()) {
    std::cout << "no wwt" << '\n';
    return 0;
  }
  mp.erase(p);
  for (auto i = mp.begin(); i != mp.end(); i++) {
    std::cout << i->first << " " << i->second << '\n';
  }
  return 0;
}
