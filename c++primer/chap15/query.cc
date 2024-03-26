#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <ostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>
class QueryResult {
public:
  QueryResult() = default;
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
  std::shared_ptr<std::vector<std::string>> &get_file() { return text; }

private:
  std::shared_ptr<std::set<int>> rp;
  std::string target;
  std::shared_ptr<std::vector<std::string>> text;
};

class TextQuery {

public:
  using line_no = int;
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
class Query;
class Query_base {
  friend class Query;

protected:
  using line_no = TextQuery::line_no;
  virtual ~Query_base() = default;

private:
  virtual QueryResult eval(TextQuery &t) const = 0;
  virtual std::string rep() const = 0;
};
class WordQuery : public Query_base {
  friend class Query;
  WordQuery(const std::string &s) : query_word(s) {}
  QueryResult eval(TextQuery &t) const override {
    return QueryResult(*t.query(query_word));
  }
  std::string rep() const override { return query_word; }
  std::string query_word;
};
class Query {
  friend Query operator~(const Query &);
  friend Query operator|(const Query &, const Query &);
  friend Query operator&(const Query &, const Query &);

public:
  Query(const std::string &str) : q(new WordQuery(str)) {}
  QueryResult eval(TextQuery &t) const { return q->eval(t); }
  std::string rep() const { return q->rep(); }

private:
  std::shared_ptr<Query_base> q;
  Query(std::shared_ptr<Query_base> query) : q(query) {}
};
class BinaryQuery : public Query_base {
protected:
  BinaryQuery(const Query &l, const Query &r, std::string s)
      : lhs(l), rhs(r), op(s) {}
  std::string rep() const override {
    return "(" + lhs.rep() + " " + op + " " + rhs.rep() + ")";
  }
  Query lhs, rhs;
  std::string op;
};
class AndQuery : public BinaryQuery {
  friend Query operator&(const Query &, const Query &);
  AndQuery(const Query &lhs, const Query &rhs) : BinaryQuery(lhs, rhs, "&") {}
  QueryResult eval(TextQuery &t) const override {
    auto right = rhs.eval(t), left = lhs.eval(t);
    auto ret_lines = std::make_shared<std::set<line_no>>();
    std::set_intersection(right.begin(), right.end(), left.begin(), left.end(),
                          std::inserter(*ret_lines, ret_lines->begin()));
    return QueryResult(rep(), ret_lines, left.get_file());
  }
};
inline Query operator&(const Query &lhs, const Query &rhs) {
  return std::shared_ptr<Query_base>(new AndQuery(lhs, rhs));
}

class OrQuery : public BinaryQuery {
  friend Query operator|(const Query &, const Query &);
  OrQuery(const Query &left, const Query &right)
      : BinaryQuery(left, right, "|") {}
  QueryResult eval(TextQuery &text) const override {
    auto right = rhs.eval(text), left = lhs.eval(text);
    auto ret_lines =
        std::make_shared<std::set<line_no>>(left.begin(), left.end());
    ret_lines->insert(right.begin(), right.end());
    return QueryResult(rep(), ret_lines, left.get_file());
  }
};
inline Query operator|(const Query &lhs, const Query &rhs) {
  return std::shared_ptr<Query_base>(new OrQuery(lhs, rhs));
}
class NotQuery : public Query_base {
  friend Query operator~(const Query &);

  NotQuery(const Query &q) : query(q) {}
  std::string rep() const { return "~(" + query.rep() + ")"; }
  QueryResult eval(TextQuery &t) const {
    auto result = query.eval(t);
    auto ret_lines = std::make_shared<std::set<int>>();
    auto beg = result.begin(), end = result.end();
    auto sz = result.get_file()->size();
    for (int i = 0; i != sz; ++i) {
      if (beg == end || *beg != i) {
        ret_lines->insert(i);
      } else if (beg != end) {
        ++beg;
      }
    }
    return QueryResult(rep(), ret_lines, result.get_file());
  }

  Query query;
};

inline Query operator~(const Query &operand) {
  return std::shared_ptr<Query_base>(new NotQuery(operand));
}

std::ostream &operator<<(std::ostream &os, const Query &query) {
  return os << query.rep();
}
int main(int argc, char *argv[]) {
  std::cout << "enter expression\n";
  std::ifstream ifs("words.txt");
  auto t = TextQuery(ifs);
  Query q = Query("talps") | Query("wwt");
  auto i = q.eval(t);
  std::cout << q.rep() << '\n';
  for (auto iter = i.begin(); iter != i.end(); iter++) {
    std::cout << *iter << " ";
  }
  std::cout << "\n";
  return 0;
}
