#include <iostream>
#include <string>
using namespace std;
int main(int argc, char const *argv[])
{
    int x[10], *p = x;
    cout << sizeof(x) / sizeof(*p) << endl;
    cout << sizeof(p) / sizeof(*p) << endl;
}
