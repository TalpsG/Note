#include <iostream>
#include <string>
using namespace std;
int main(int argc, char const *argv[])
{
    string s = "word";
    string p1 = s + (s[s.size() - 1] == 's' ? "" : "s");
}
