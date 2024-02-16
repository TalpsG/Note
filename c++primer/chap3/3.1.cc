#include "../chap2/Sales_data.h"
#include <iostream>
int main() {
  struct Sales_data t1, t2;
  std::cin >> t1.bookNo >> t1.units_sold >> t1.revenue;
  std::cin >> t2.bookNo >> t2.units_sold >> t2.revenue;
  if (t1.bookNo != t2.bookNo) {
    std::cout << "bookNo is not same\n";
    return 0;
  }
  std::cout << t1.bookNo << " " << t1.units_sold + t2.units_sold << " "
            << t1.units_sold * t1.revenue + t2.units_sold * t2.revenue << "\n";
  return 0;
}
