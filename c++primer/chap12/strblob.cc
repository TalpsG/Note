#include <cstddef>
#include <fstream>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
class strBlobPtr;
class strBlob {
  friend class strBlobPtr;

public:
  typedef std::vector<std::string>::size_type size_type;

  // constructor
  strBlob() : data(std::make_shared<std::vector<std::string>>()){};
  // copy constructor
  strBlob(const strBlob &sb)
      : data(std::make_shared<std::vector<std::string>>(
            *new std::vector<std::string>(*sb.data))) {}

  explicit strBlob(std::initializer_list<std::string> i1)
      : data(std::make_shared<std::vector<std::string>>(i1)){};

  strBlob &operator=(const strBlob &sb) {
    data = std::make_shared<std::vector<std::string>>(*sb.data);
    return *this;
  }
  size_type size() const { return data->size(); }
  bool empty() const { return data->empty(); }
  void push_back(const std::string &str) { data->push_back(str); }
  void push_back(std::string &&str) { data->push_back(str); }
  void pop_back() {
    check(0, "pop_back");
    return data->pop_back();
  };
  std::string &font() const {
    check(0, "front const");
    return data->front();
  };
  std::string &font() {
    check(0, "front");
    return data->front();
  };

  std::string &back() const {
    check(0, "back");
    return data->back();
  };
  strBlobPtr begin();
  strBlobPtr end();

private:
  std::shared_ptr<std::vector<std::string>> data;
  void check(size_type i, const std::string &str) const {
    if (i >= data->size()) {
      throw std::out_of_range(str);
    }
  };
};

class strBlobPtr {
  friend bool eq(const strBlobPtr &p1, const strBlobPtr &p2);

public:
  strBlobPtr() : curr(0) {}
  strBlobPtr(strBlob &a, std::size_t sz = 0) : wptr(a.data), curr(sz) {}
  strBlobPtr(const strBlob &a, std::size_t sz = 0) : wptr(a.data), curr(sz) {}
  std::string &deref() const;
  strBlobPtr &incr();

private:
  std::shared_ptr<std::vector<std::string>> check(std::size_t,
                                                  const std::string &str) const;
  std::weak_ptr<std::vector<std::string>> wptr;
  std::size_t curr;
};
std::shared_ptr<std::vector<std::string>>
strBlobPtr::check(std::size_t i, const std::string &str) const {
  auto ret = wptr.lock();
  if (!ret) {
    throw std::runtime_error("unbound strBlobPtr");
  }
  if (i >= ret->size()) {
    throw std::out_of_range(str);
  }
  return ret;
}
std::string &strBlobPtr::deref() const {
  auto p = check(curr, "deref");
  return (*p)[curr];
}
strBlobPtr &strBlobPtr::incr() {
  check(curr, "incr");
  ++curr;
  return *this;
}
strBlobPtr strBlob::begin() { return strBlobPtr(*this); };
strBlobPtr strBlob::end() {
  auto ret = strBlobPtr(*this, this->size());
  return ret;
};
bool eq(const strBlobPtr &p1, const strBlobPtr &p2) {
  auto sp1 = p1.wptr.lock(), sp2 = p2.wptr.lock();
  if (sp1 == sp2 && p1.curr == p2.curr) {
    return true;
  }
  return false;
}
int main(int argc, char *argv[]) {
  std::ifstream ifs("strblob.cc");
  std::string line;
  auto p = std::make_shared<strBlob>();
  while (std::getline(ifs, line)) {
    p->push_back(line);
  }
  for (auto i = p->begin(); !eq(i, p->end()); i.incr()) {
    std::cout << i.deref() << "\n";
  }
  // strBlob b1(*p);
  // printf("%p\n%p\n", p.get(), &b1);
}
