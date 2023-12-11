#include "Person.hpp"
std::string Person::getName() const
{
    return this->name;
}
std::string Person::getAdd() const
{
    return this->address;
}

std::istream &read(std::istream &is, Person &p)
{
    is >> p.name >> p.address;
    return is;
}
std::ostream &print(std::ostream &os, const Person &p)
{
    os << p.name << p.address;
    return os;
}
Person::Person(std::istream &is)
{
    read(is, *this);
}
void X::wwt(int i)
{
    std::cout << i << std::endl;
}