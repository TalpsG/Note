#include <iostream>
#include <vector>
using namespace std;
int main()
{
    int a[]{1, 2, 3, 4, 5};
    vector<int> a2(begin(a), end(a));

    return 0;
}