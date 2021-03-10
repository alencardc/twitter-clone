#ifndef __WORK_ITEM_HPP__
#define __WORK_ITEM_HPP__

#include "lib/socket/TCPConnection.hpp"

class WorkItem {
  TCPConnection* m_connection;

  public:
    WorkItem(TCPConnection* connection);
    ~WorkItem();

    TCPConnection* getConnection();
};

#endif