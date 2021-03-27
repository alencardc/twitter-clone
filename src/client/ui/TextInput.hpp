#ifndef __TextInput_hpp__
#define __TextInput_hpp__
#include <string>
#include "Vector2.hpp"
#include "Widget.hpp"


class TextInput : public Widget {
  friend class Window;
  std::string m_text;
  int m_maxSize;
  Vector2 m_position;
  Vector2 m_size;

 public:

  TextInput(Vector2 pos, Vector2 size);
  std::string text();
  void setMaxSize(int size);

  std::string input(Window &win);

 private:
  void draw(Window &win);
};

#endif  // __TextInput_hpp__