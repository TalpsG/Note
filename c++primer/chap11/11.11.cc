#include <set>
bool cmp(int a, int b) { return a < b; }
int main(int argc, char *argv[]) {
  std::set<int, bool (*)(int, int)> s(cmp);
  return 0;
}
