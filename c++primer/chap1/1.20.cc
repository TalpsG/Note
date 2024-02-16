#include "Sales_item.h"
#include <iostream>
using namespace std;
int main() {
  int n;
  cin >> n;
  for (int i = 0; i < n; i++) {
    Sales_item temp;
    cin >> temp;
    cout << temp << "\n";
  }
  return 0;
}
