#include <iostream>
#include <vector>
using namespace std;
int main(int argc, char const *argv[])
{
    vector<int> vec;
    for (int i = 0; i < 10; i++)
    {
        vec.push_back(i);
    }
    for (auto i = vec.begin(); i != vec.end(); i++)
    {
        *i *= 2;
    }
    for (int i = 0; i < 10; i++)
    {
        cout << vec[i] << "\n";
    }
    return 0;
}
