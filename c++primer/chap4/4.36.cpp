#include <iostream>
#include <string>
using namespace std;
int main(int argc, char const *argv[])
{
    int i;
    double d;
    const string *ps;
    char *pc;
    void *pv;
    pv = static_cast<void *>(const_cast<string *>(ps));
    i = static_cast<int>(*pc);
    pv = static_cast<double *>(&d);
    pc = static_cast<char *>(pv);
    return 0;
}
