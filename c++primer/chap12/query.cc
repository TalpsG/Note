#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <ostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>
class QueryResult {
public:
  QueryResult(const std::string &s, std::shared_ptr<std::set<int>> st,
              std::shared_ptr<std::vector<std::string>> txt)
      : rp(st), target(s), text(txt) {}
  std::ostream &print(std::ostream &os) {
    for (auto i : *rp) {
      os << i << " ";
    }
    return os;
  }
  std::set<int>::iterator begin() { return rp->begin(); }
  std::set<int>::iterator end() { return rp->end(); }
  std::shared_ptr<std::vector<std::string>> get_file() { return text; }

private:
  std::shared_ptr<std::set<int>> rp;
  std::string target;
  std::shared_ptr<std::vector<std::string>> text;
};

class TextQuery {

public:
  TextQuery(std::ifstream &ifs);
  std::shared_ptr<QueryResult> query(const std::string &s) {
    std::shared_ptr<std::set<int>> p = nullptr;
    if (mp.find(s) == mp.end()) {
      p = std::make_shared<std::set<int>>();
    } else {
      p = mp[s];
    }

    return std::make_shared<QueryResult>(s, p, text);
  }

private:
  std::map<std::string, std::shared_ptr<std::set<int>>> mp;
  std::shared_ptr<std::vector<std::string>> text;
};

TextQuery::TextQuery(std::ifstream &ifs)
    : text(std::make_shared<std::vector<std::string>>()) {
  std::string line;
  int ln = 1;
  while (std::getline(ifs, line)) {
    (*text).push_back(line);
    std::istringstream iss(line);
    while (iss >> line) {
      if (mp.find(line) == mp.end()) {
        mp.insert({line, std::make_shared<std::set<int>>()});
      }
      mp[line]->insert(ln);
    }
    ++ln;
  }
}

void print(std::ostream &os, QueryResult &rs) { rs.print(os); }
void runQueries(std::ifstream &ifs) {
  TextQuery tq(ifs);
  while (true) {
    std::cout << "enter a word, q for quit: ";
    std::string word;
    if ((!(std::cin >> word)) || word == "q") {
      break;
    }
    tq.query(word)->print(std::cout) << "\n";
  }
}
int main(int argc, char *argv[]) {
  std::ifstream ifs("query.cc");
  runQueries(ifs);
  return 0;
}
