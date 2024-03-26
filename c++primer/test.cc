#include <string>
#include <unordered_map>
std::unordered_map<int, std::string> map;
const std::string &getValue(int key) {
  auto iter = map.find(key);
  if (iter != map.end()) {
    return iter->second;
  }
  return "null";
}
int main(int argc, char *argv[]) {
  for (int i = 0; i < 100; ++i) {
    map[i] = std::to_string(i * (i + 1));
  }

  std::string temp = getValue(8);
  return 0;
}
