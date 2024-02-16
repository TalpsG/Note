#include "Sales_item.h"
#include <iostream>
using namespace std;
int main() {
  int n;
  cin >> n;
  Sales_item sum;
  for (int i = 0; i < n; i++) {
    if (i == 0) {
      cin >> sum;
    } else {
      Sales_item t;
      cin >> t;
      sum += t;
    }
  }
  cout << sum << "\n";
  return 0;
}
