#include "Person.hpp"
#include <vector>
#include <iostream>
class example
{
public:
    static double rate;
    static const int vecsize = 20;
    static std::vector<double> vec;
};
double example::rate;
std::vector<double> example::vec;
int main()
{
    std::cout << example::rate << std::endl;
    std::cout << example::vecsize << std::endl;
}