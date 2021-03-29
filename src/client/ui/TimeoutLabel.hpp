#ifndef __TimeoutLabel_hpp__
#define __TimeoutLabel_hpp__

#include <string>
#include <mutex>
#include "Vector2.hpp"
#include "Widget.hpp"

class TimeoutLabel : public Widget {
  friend class Window;
  std::string m_text;
  long int m_timeoutSeconds;
  long int m_init;

  std::mutex m_mutex;
 public:
  Vector2 position;
  TimeoutLabel(Vector2 pos, const char *text);

  
  void setTimeout(long int seconds);
  void setText(std::string);

 private:
  void draw(Window &win);
};

#endif // __TimeoutLabel_hpp__