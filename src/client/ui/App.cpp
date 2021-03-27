#include "App.hpp"

App::App(): m_windows() {
  initscr();
  noecho();
  keypad(stdscr, TRUE);
  halfdelay(5);
  m_screen = stdscr;
}

App::~App() {
  for (auto window : m_windows) {
    delete window;
  }
  endwin();
}

int App::maxX() {
  return COLS;
}
int App::maxY() {
  return LINES;
}

Window* App::createWindow(Vector2 pos, Vector2 size) {
  m_windows.push_back(new Window(pos, size));
  return m_windows.back();
}

void App::erase() {
  werase(m_screen);
}

void App::draw() {
  for (auto const &window : m_windows) {
    window->draw();
  }
  wrefresh(m_screen);
}

int App::getKeyEvent() {
  return wgetch(m_screen);
}