#include <iostream>
using namespace std;
int main()
{
    int a[3][4];
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            a[i][j] = i + j;
        }
    }
    printf("foreach\n");
    for (int(&i)[4] : a)
    {
        for (int k : i)
        {
            cout << k << " ";
        }
        cout << endl;
    }
    cout << "for ref" << endl;
    for (int(*i)[4] = begin(a); i != end(a); i++)
    {
        for (int *j = begin(*i); j != end(*i); j++)
        {
            cout << *j << " ";
        }
        cout << endl;
    }
    return 0;
}