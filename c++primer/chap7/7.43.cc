class NoDefault {
public:
  NoDefault(int a){};
};
class C {
public:
  NoDefault a;
  C(int i = 1) : a(i){};
};
int main(int argc, char *argv[]) {
  C c;
  return 0;
}
