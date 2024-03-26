#include <memory>
struct dest {};
struct conn {};
conn connect(dest *) {
  conn c;
  return c;
}
void disconnect(conn c) {}
void f(dest &d) {
  conn c = connect(&d);
  std::shared_ptr<conn> p(&c, [](conn c) -> void { disconnect(c); });
}
