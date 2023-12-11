#include <iostream>
#include <vector>
#include <cstdio>
#include "wwt.hpp"
using namespace std;
void swap(int &a, int &b)
{
    int temp = a;
    a = b;
    b = temp;
}
void check(const int &a)
{
    ;
}
void check_arr(int (&a)[10])
{
    ;
}
int comparew(const int a, const int *const p)
{
    return a > *p;
}
void swap_p(int *&p1, int *&p2)
{
    printf("before:%p,%p\n", p1, p2);
    int *p3 = p1;
    p1 = p2;
    p2 = p3;
    printf("after:%p,%p\n", p1, p2);
}
void sum(initializer_list<int> a)
{
    int sum = 0;
    for (int i : a)
    {
        sum += i;
    }
    cout << sum << endl;
}
int arr[5] = {1, 2, 3, 4, 5};
string (*func())[10];
int (&func(int i))[5]
{
    return arr;
}
void default_para2(int a = 3, int b = 4);

void default_para2(int a, int b)
{
    cout << a << " " << b << endl;
    ;
}
int wwt(int, int)
{
    return 1;
}
int main(int argc, char const *argv[])
{
    vector<int (*)(int, int)> vec(10, wwt);
    cout << vec[3](3, 4);

    return 0;
}
