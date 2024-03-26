#include <iostream>
#include <string>
class HasPtr {
public:
  int a;
  std::string *sp;
  HasPtr() : a(), sp(new std::string()){};
  HasPtr(int i, std::string str) : a(i), sp(new std::string(str)){};
  HasPtr(HasPtr &hp) : a(hp.a), sp(new std::string(*hp.sp)) {}
  HasPtr &operator=(HasPtr &hp) {
    a = hp.a;
    delete sp;
    sp = new std::string(*hp.sp);
    return *this;
  }
  ~HasPtr() { delete sp; }
};
int main(int argc, char *argv[]) {
  HasPtr h1{1, "wwt"};
  HasPtr h2{2, "talps"};
  HasPtr h3 = h1;
  HasPtr h4(h2);
  std::cout << h1.a << " " << *h1.sp << '\n';
  std::cout << h2.a << " " << *h2.sp << '\n';
  std::cout << h3.a << " " << *h3.sp << '\n';
  std::cout << h4.a << " " << *h4.sp << '\n';
  return 0;
}
