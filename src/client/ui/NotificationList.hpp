#ifndef __NotificationList_hpp__
#define __NotificationList_hpp__
#include <string>
#include <list>
#include <mutex>
#include "Vector2.hpp"
#include "Widget.hpp"
#include "client/Notification.hpp"

class NotificationList : public Widget {
  friend class Window;
  Vector2 m_position;
  Vector2 m_size;
  std::list<Notification> m_list;
  bool m_isActive;
  long unsigned m_showingBegin;

  std::mutex m_mutex;


 public:
  NotificationList(Vector2 pos, Vector2 size);

  void addItem(Notification item);
  void showNext();
  void showPrevious();
  int itemsPerPage();
  int linesPerItem();

 private:
  void draw(Window &win);
};

#endif  // __NotificationList_hpp__