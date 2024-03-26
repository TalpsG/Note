#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
using namespace std;
vector<string> stringSplit(const string &str) {
  vector<string> tokens;
  istringstream iss(str);
  string temp;
  while (iss >> temp) {
    tokens.push_back(temp);
  }
  return tokens;
}
void printTime(const string &input) {
  int clk, minutes, d_clk, d_minutes;
  char chartemp;
  string strtemp, PM;
  istringstream iss(input);
  iss >> clk >> chartemp >> minutes >> PM >> d_clk >> strtemp >> d_minutes;
  minutes += d_minutes;
  if (minutes >= 60) {
    minutes %= 60;
    ++clk;
  }
  clk += d_clk;
  if (clk >= 12) {
    clk %= 12;
    PM = (PM == "AM") ? "PM" : "AM";
  }
  cout << clk << ":";
  cout << setfill('0') << setw(2) << minutes;
  cout << " " << PM;
}
string promptUserForFile(ifstream &ifs, string promt = "",
                         string repromt = "") {
  string filename;
  cout << promt << " ";
  if (!(cin >> filename)) {
    throw runtime_error("cin error");
  }
  while (1) {
    ifs.open(filename);
    if (ifs.fail()) {
      cout << repromt << " ";
      if (!(cin >> filename)) {
        throw runtime_error("cin error");
      }
      continue;
    }
    return filename;
  }
}
int main(int argc, char *argv[]) {
  auto price = make_pair(3.4, 5);
  auto values = make_tuple(3, 4, "hi");
  auto [a, b] = price;
  const auto &[x, y, z] = values;
  cout << a << " " << b << "\n";
  cout << x << " " << y << " " << z << "\n";

  return 0;
}
