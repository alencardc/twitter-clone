#include "server/WorkItem.hpp"

WorkItem::WorkItem(TCPConnection* connection) {
  m_connection = connection;
}
WorkItem::~WorkItem() {
  delete m_connection;
}

TCPConnection* WorkItem::getConnection() {
  return m_connection;
}