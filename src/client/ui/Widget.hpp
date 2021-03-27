#ifndef __Widget_hpp__
#define __Widget_hpp__

class Window;

class Widget {
  friend class Window;

 public:
  virtual ~Widget() = default;

 private:
  virtual void draw(Window &win) = 0;
};

#endif // __Widget_hpp__

