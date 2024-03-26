#include <iostream>
#include <string>
class Employee {
private:
  static int cnt;
  std::string name;

public:
  int id;
  Employee() : name(""), id(++cnt) {}
  Employee(const std::string &n) : name(n), id(++cnt) {}
  Employee(Employee &e) : name(e.name), id(++cnt) {}
  Employee &operator=(Employee &e) { name = e.name; }
};
int Employee::cnt = 0;
int main(int argc, char *argv[]) {
  Employee e1;
  Employee e2("wwt");
  std::cout << e1.id << "\n";
  std::cout << e2.id << "\n";
  return 0;
}
