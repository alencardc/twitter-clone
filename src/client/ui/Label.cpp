#include "Label.hpp"
#include "Window.hpp"

Label::Label(Vector2 pos, const char *text): position(pos), text(text) {}

void Label::draw(Window &win) {
  mvwprintw(win.window(), position.y, position.x, text.c_str());
}