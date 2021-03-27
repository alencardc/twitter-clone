#ifndef __App_hpp__
#define __App_hpp__

#include <vector>
#include <ncurses.h>
#include "Window.hpp"

class App {
  std::vector<Window*> m_windows;
  WINDOW* m_screen;

  public:
    App();
    ~App();

    int maxX();
    int maxY();

    Window* createWindow(Vector2 pos, Vector2 size);
    void draw();
    void erase();

    int getKeyEvent();
};

#endif // __App_hpp__