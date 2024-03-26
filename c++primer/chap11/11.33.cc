#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <ostream>
#include <sstream>
#include <string>

auto buildmap(std::ifstream &ifs) -> std::map<std::string, std::string> {
  std::map<std::string, std::string> mp;
  std::string key, value;
  while (ifs >> key && std::getline(ifs, value)) {
    if (value.size() >= 1) {
      mp[key] = value.substr(1);
    } else {
      std::cout << "invalid rules\n";
      exit(0);
    }
  }
  return mp;
}
auto word_transform(const std::string &str,
                    const std::map<std::string, std::string> &mp)
    -> const std::string & {
  auto pos = mp.find(str);
  if (pos == mp.end())
    return str;
  else
    return pos->second;
}
int main(int argc, char *argv[]) {
  std::ifstream ifs("rules.txt");
  std::ifstream words("words.txt");
  auto mp = buildmap(ifs);
  std::string temp;
  while (std::getline(words, temp)) {
    std::istringstream iss(temp);
    bool flag = true;
    while (iss >> temp) {
      if (flag) {
        flag = false;
      } else {
        std::cout << ' ' << std::flush;
      }
      std::cout << word_transform(temp, mp) << std::flush;
    }
    std::cout << '\n';
  }
  return 0;
}
