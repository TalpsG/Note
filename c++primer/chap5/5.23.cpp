#include <iostream>
using namespace std;
int main(int argc, char const *argv[])
{
    int a, b, c;
    cin >> a >> b;
    try
    {
        cout << a / b;
        /* code */
    }
    catch (exception exc)
    {
        cout << exc.what() << endl;
    }

    return 0;
}
