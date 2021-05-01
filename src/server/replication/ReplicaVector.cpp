#include "ReplicaVector.hpp"

ReplicaInfo& ReplicaVector::operator[](const int index) {
  return m_vector[index];
}

ReplicaInfo ReplicaVector::createAndInsert(ReplicaInfo newReplica) {
  std::lock_guard<std::mutex> lock(m_mutex);
  newReplica.id = getNextId();
  m_vector.push_back(newReplica);
  m_cv.notify_one(); 
  return newReplica;
}

void ReplicaVector::insert(ReplicaInfo newReplica) {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_vector.push_back(newReplica);
  m_cv.notify_one(); 
}

void ReplicaVector::setVector(std::vector<ReplicaInfo> vector) {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_vector = vector;
  m_cv.notify_one(); 
}

std::vector<ReplicaInfo> ReplicaVector::toVector() {
  return m_vector;
}

std::vector<ReplicaInfo>::iterator ReplicaVector::begin() {
    return m_vector.begin();
}

std::vector<ReplicaInfo>::iterator ReplicaVector::end() {
    return m_vector.end();
}

int ReplicaVector::getNextId() {
  return m_vector.size();
}