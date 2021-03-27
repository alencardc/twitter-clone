#ifndef __Window_hpp__
#define __Window_hpp__

#include <vector>
#include <string>
#include <ncurses.h>
#include "Vector2.hpp"
#include "Widget.hpp"

class Window {
  Vector2 m_position;
  Vector2 m_size;
  WINDOW* m_window;
  std::string m_name;
  std::vector<Widget*> m_widgets;

  friend class App;

  public:
    Window(Vector2 position, Vector2 size);
    ~Window();

    Vector2 position();
    Vector2 size();

    WINDOW* window();

    void addName(const char* name);
    void addWidget(Widget* widget);

    int getKeyEvent();

  private: 
    void draw();
};

#endif // __Window_hpp__