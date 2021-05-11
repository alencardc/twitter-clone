#include <ncurses.h>
#include <math.h>
#include "Window.hpp"
#include "NotificationList.hpp"
#include "lib/utils/string.hpp"


NotificationList::NotificationList(Vector2 pos, Vector2 size):
  m_position(pos),
  m_size(size),
  m_list(),
  m_isActive(false),
  m_showingBegin(0)
{}

void NotificationList::addItem(Notification item) {
  m_mutex.lock();
  m_list.push_front(item);
  m_mutex.unlock();
}

void NotificationList::showNext() {
  m_mutex.lock();
  if (m_showingBegin < m_list.size()-itemsPerPage()) {
    m_showingBegin += 1;
  }
  m_mutex.unlock();
}

void NotificationList::showPrevious() {
  m_mutex.lock();
  if (m_showingBegin > 0) {
    m_showingBegin -= 1;
  }
  m_mutex.unlock();
}

int NotificationList::linesPerItem() {
  return 1 + 2 + ceil(Notification::MAX_SIZE / (float)m_size.x);
}

int NotificationList::itemsPerPage() {
  return m_size.y / linesPerItem(); 
}

void NotificationList::draw(Window &win) {
  m_mutex.lock();
  printf("Size: %ld\n", m_list.size());
  int i = 0;
  auto it = m_list.begin();
  std::advance(it, m_showingBegin);

  int printBegin = m_position.y;
  while (it != m_list.end() && i < itemsPerPage()) {
    // Clear
    for (int j = 0; j < linesPerItem(); j++)
      mvwprintw(win.window(), printBegin+j, m_position.x, std::string(m_size.x, ' ').c_str());

    Notification n = *it;
    std::string header = n.sender + "  --  " + n.date;
    mvwprintw(win.window(), printBegin, m_position.x, header.c_str());
    
    std::string message = n.message.size() > Notification::MAX_SIZE 
      ? n.message.erase(Notification::MAX_SIZE) 
      : n.message;
    
    std::vector<std::string> lines = splitByLength(m_size.x, message);
    for (long unsigned j = 0; j < lines.size(); j++) {
      mvwprintw(win.window(), printBegin+1+j, m_position.x, lines[j].c_str());
    }
    printBegin += linesPerItem();

    i++; it++;
  }

  m_mutex.unlock();
}