/*
Assignment 2: Marriage Pact

This assignment asks you to discover your one true love using containers and
pointers. There are two parts: implement get_applicants and find_matches.

Submit to Paperless by 11:59pm on 2/1/2024.
*/

#include <fstream>
#include <iostream>
#include <iterator>
#include <queue>
#include <set>
#include <string>

std::string girls_list = "./students.txt";

std::set<std::string> get_applicants(std::string filename) {
  // TODO: Implement this function. Feel free to change std::set to
  // std::unordered_set if you wish!
  std::ifstream ifs(filename);
  std::set<std::string> girls;
  std::string line;
  while (std::getline(ifs, line)) {
    girls.insert(line);
    if (line[0] == 'W')
      std::cout << line << "\n";
  }
  return girls;
}

std::queue<std::string *> find_matches(std::set<std::string> &students) {
  // TODO: Implement this function.
  std::queue<std::string *> q;
  return q;
}

int main() {
  // Your code goes here. Don't forget to print your true love!
  get_applicants(girls_list);

  return 0;
}
