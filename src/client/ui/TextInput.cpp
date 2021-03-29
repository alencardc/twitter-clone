#include "TextInput.hpp"
#include "Window.hpp"
#include "lib/utils/string.hpp"

TextInput::TextInput(Vector2 pos, Vector2 size):
  m_text(""),
  m_maxSize(-1),
  m_position(pos),
  m_size(size)
{}

std::string TextInput::input(Window &win) {
  int event;
  while ((event = win.getKeyEvent()) != KEY_F(1)) {
    if (event > 0 && event < 256) {
      char charBuffer[2] = { (char) event, '\0' };

      if (charBuffer[0] == '\n') {
        std::string result = m_text;
        m_text.clear();
        return result;
      } else if (m_text.size() < (long unsigned)m_maxSize) {
        m_text.append(charBuffer);
      }
    } else if (event == KEY_BACKSPACE) {
      if (m_text.empty() == false)
        m_text.pop_back();
    }
    draw(win);
  }
  return "";
}

void TextInput::draw(Window &win) {
  for (int j = 0; j < m_size.y; j++)
    mvwprintw(win.window(), m_position.y+j, m_position.x, std::string(m_size.x, ' ').c_str());
  
  std::vector<std::string> lines = splitByLength(m_size.x, m_text);
  for (long unsigned j = 0; j < lines.size(); j++) {
    mvwprintw(win.window(), m_position.y+j, m_position.x, lines[j].c_str());
  }
}

void TextInput::setMaxSize(int size) {
  m_maxSize = size;
}