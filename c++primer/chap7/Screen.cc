#include "Window.h"
char Screen::get(pos r, pos c) const {
  pos row = r * width;
  return contents[row + c];
}
Screen &Screen::set(pos r, pos c, char a) {
  pos row = r * width;
  contents[row + c] = a;
  return *this;
}
Screen &Screen::set(char a) {
  contents[cursor] = a;
  return *this;
}
inline Screen &Screen::move(pos r, pos c) {
  pos row = r * width;
  cursor = row + c;
  return *this;
}

Screen::pos Screen::size() const { return height * width; }
