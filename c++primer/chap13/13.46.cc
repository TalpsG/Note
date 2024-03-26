#include <vector>
int f() { return 0; }
int main(int argc, char *argv[]) {
  std::vector<int> vi(100);
  int &&r1 = f();
  int &r2 = vi[2];
  r1 = 4;
  int &&r4 = vi[0] * f();
  return 0;
}
