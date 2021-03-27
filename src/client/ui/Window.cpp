#include "Window.hpp"

Window::Window(Vector2 position, Vector2 size): 
  m_position(position),
  m_size(size),
  m_name("")
{
  m_window = newwin(size.y, size.x, position.y, position.x);
  noecho();
  keypad(m_window, TRUE);
}

Window::~Window() {
  for (auto widget : m_widgets) {
    delete widget;
  }
}

Vector2 Window::position() {
  return m_position;
}


Vector2 Window::size() {
  return m_size;
}

WINDOW* Window::window() {
  return m_window;
}

void Window::draw() {
  //werase(m_window);
  for (auto &widget : m_widgets) {
    widget->draw(*this);
  }
  box(m_window, 0, 0);
  if (m_name.empty() == false) {
    mvwprintw(m_window, 0, 2, m_name.c_str());
  }
  wrefresh(m_window);
}

void Window::addName(const char* name) {
  m_name = name;
}

void Window::addWidget(Widget* widget) {
  m_widgets.push_back(widget);
}

int Window::getKeyEvent() {
  return wgetch(m_window);
}