#include <iostream>
#include <string>
using namespace std;
int main(int argc, char const *argv[])
{
    string a, b;
    string m;
    int t = 0;
    int max = -1;
    while ((cin >> a))
    {
        if (b.empty())
        {
            b = a;
            continue;
        }
        if (a == b)
        {
            ++t;
            max = t > max ? t : max;
            m = a;
        }
        else
        {
            b = a;
            t = 0;
        }
    }
    cout << max << " " << m << endl;
    return 0;
}
