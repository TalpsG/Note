#include "Window.h"
void Window_mgr::clear(ScreenIndex index) {
  Screen &s = screens[index];
  s.contents = std::string(s.height * s.width, ' ');
}
