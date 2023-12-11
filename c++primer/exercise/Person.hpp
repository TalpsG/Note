#ifndef PERSON_H
#define PERSON_H
#include <string>
#include <iostream>
class Person
{
    friend class X;
    friend std::istream &read(std::istream &is, Person &p);
    friend std::ostream &print(std::ostream &os, const Person &p);

private:
    std::string name;
    std::string address;

public:
    Person() = default;
    Person(const std::string &name) : name(name) {}
    Person(const std::string &name, const std::string &add) : name(name), address(add) {}
    Person(std::istream &is);
    std::string getName() const;
    std::string getAdd() const;
};
class X;
class Y;
class X
{
public:
    void wwt(int i);
};
class Y
{
    friend void X::wwt(int i);

private:
    int id;
};
#endif