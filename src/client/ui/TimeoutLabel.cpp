#include <time.h>
#include "TimeoutLabel.hpp"
#include "Window.hpp"


TimeoutLabel::TimeoutLabel(Vector2 pos, const char *text): 
  m_text(text),
  m_timeoutSeconds(2),
  m_init(0),
  position(pos)
{}

void TimeoutLabel::draw(Window &win) {
  m_mutex.lock();
  if (m_text.empty() == false) {
    if (m_timeoutSeconds+m_init > time(NULL)){
      mvwprintw(win.window(), position.y, position.x, m_text.c_str());
    }else
      mvwprintw(win.window(), position.y, position.x, std::string(m_text.size(), ' ').c_str());
  }
  
  m_mutex.unlock();
}

void TimeoutLabel::setTimeout(long int seconds) {
  m_mutex.lock();
  m_timeoutSeconds = seconds;
  m_mutex.unlock();
}

void TimeoutLabel::setText(std::string text) {
  m_mutex.lock();
  m_init = time(NULL);
  m_text = text;
  m_mutex.unlock();
}