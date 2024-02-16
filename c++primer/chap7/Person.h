#include <iostream>
#include <istream>
#include <ostream>
#include <string>
class Person {
  friend std::ostream &print(std::ostream &os, const Person &p);
  friend std::istream &read(std::istream &is, Person &p);

public:
  Person() = default;
  Person(std::istream &is);
  Person(const std::string &s) : name(s){};
  Person(const std::string &n, const std::string &a) : name(n), address(a){};
  std::string getadd() const { return address; };
  std::string getname() const { return name; };
  void setname(const std::string &n) { name = n; };
  void setadd(const std::string &a) { address = a; };

private:
  std::string name;
  std::string address;
};

std::ostream &print(std::ostream &os, const Person &p);
std::istream &read(std::istream &is, Person &p);
