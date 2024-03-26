#include <cstring>
#include <string>
int main(int argc, char *argv[]) {
  std::string a{"talps"};
  std::string b{"wwt"};
  char *p = new char[a.size() + b.size() + 1]();
  strcpy(p, (a + b).c_str());
  printf("%s\n", p);
  return 0;
}
