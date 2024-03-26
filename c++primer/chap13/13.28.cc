#include <string>
class TN {
  TN(TN &tn) : value(tn.value), count(1), left(tn.left), right(tn.right) {
    ++left->count;
    ++right->count;
  }
  TN &operator=(TN &tn) {
    value = tn.value;
    count = 1;
    left = tn.left;
    right = tn.right;
    ++left->count;
    ++right->count;
    return *this;
  }

private:
  std::string value;
  int count;
  TN *left;
  TN *right;
};
class BST {
private:
  TN *root;
};
