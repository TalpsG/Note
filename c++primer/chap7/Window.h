#include <ostream>
#include <string>
#include <vector>

class Screen {
  friend class Window_mgr;

public:
  using pos = unsigned int;
  Screen() = default;
  Screen(pos h, pos w) : height(h), width(w), contents(h * w, ' '){};
  Screen(pos h, pos w, char c) : height(h), width(w), contents(h * w, c){};
  char get() const { return contents[cursor]; }
  char get(pos r, pos c) const;
  Screen &move(pos r, pos c);
  Screen &set(pos, pos, char);
  Screen &set(char);
  pos size() const;
  Screen &display(std::ostream &os) {
    do_display(os);
    return *this;
  }
  const Screen &display(std::ostream &os) const {
    do_display(os);
    return *this;
  }

private:
  void do_display(std::ostream &os) const { os << contents; }
  pos cursor = 0;
  pos height = 0, width = 0;
  std::string contents;
};

class Window_mgr {
public:
  using ScreenIndex = unsigned int;
  void clear(ScreenIndex);

private:
  std::vector<Screen> screens{Screen(24, 80, ' ')};
};
