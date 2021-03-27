#ifndef __Label_hpp__
#define __Label_hpp__
#include <string>
#include "Vector2.hpp"
#include "Widget.hpp"

class Label : public Widget {
  friend class Window;

 public:
  Label(Vector2 pos, const char *text);

  Vector2 position;
  std::string text;

 private:
  void draw(Window &win);
};

#endif  // __Label_hpp__