#include <iostream>
#include <vector>
using namespace std;
int main()
{
    vector<int> vec{1, 4, 9};
    cout << *vec.begin() << endl;
    cout << *vec.begin() + 1 << endl;
    return 0;
}