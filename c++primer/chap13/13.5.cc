#include <string>
class HasPtr {
public:
  HasPtr(const std::string &s = std::string()) : ps(new std::string(s)), i(0) {}
  HasPtr(HasPtr &p) : ps(new std::string(*(p.ps))), i(p.i) {}
  HasPtr &operator=(HasPtr &b) {
    delete ps;
    ps = new std::string(*(b.ps));
    i = b.i;
    return *this;
  }
  ~HasPtr() { delete ps; }

private:
  std::string *ps;
  int i;
};
