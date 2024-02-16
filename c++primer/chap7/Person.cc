#include "Person.h"
std::ostream &print(std::ostream &os, const Person &p) {
  os << p.name << " " << p.address;
  return os;
}
std::istream &read(std::istream &is, Person &p) {
  is >> p.name >> p.address;
  return is;
}
Person::Person(std::istream &is) { read(is, *this); }
