#include <vector>
template <typename T> class NoDefault {
public:
  NoDefault() = delete;
};
class Foo {
public:
  Foo() = delete;
};

int main(int argc, char *argv[]) {
  std::vector<Foo> vec;
  return 0;
}
