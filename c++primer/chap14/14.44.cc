#include <functional>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
std::map<std::string, std::function<int(int, int)>> mp;
void initCalculator() {
  mp.insert({"+", [](int a, int b) -> int { return a + b; }});
  mp.insert({"-", [](int a, int b) -> int { return a - b; }});
  mp.insert({"*", [](int a, int b) -> int { return a * b; }});
  mp.insert({"/", [](int a, int b) -> int {
               if (b == 0)
                 throw std::runtime_error("div 0");
               return a / b;
             }});
}

int main(int argc, char *argv[]) {
  initCalculator();
  std::string line;
  std::cout << "enter a  expr like a * b:\n";
  int a, b;
  std::string symbol;
  std::cin >> a >> symbol >> b;
  std::cout << "symbol: " << symbol << '\n';
  std::cout << "res: " << mp[symbol](a, b) << "\n";
  return 0;
}
