#ifndef __ReplicaVector_hpp__
#define __ReplicaVector_hpp__

#include <vector>
#include <mutex>
#include <condition_variable>
#include "server/ReplicaInfo.hpp"

class ReplicaVector {
  std::vector<ReplicaInfo> m_vector;
  std::mutex m_mutex;
  std::condition_variable m_cv;

  public:
    ReplicaVector() : m_vector(), m_mutex(), m_cv() {}
    ReplicaVector(const ReplicaVector& sv) : m_vector(sv.m_vector), m_mutex(), m_cv() {}
    ~ReplicaVector() {}

    ReplicaInfo& operator[](const int index);
    ReplicaInfo createAndInsert(ReplicaInfo newReplica);
    void insert(ReplicaInfo newReplica);
    void setVector(std::vector<ReplicaInfo> vector);
    std::vector<ReplicaInfo> toVector();
    std::vector<ReplicaInfo>::iterator begin();
    std::vector<ReplicaInfo>::iterator end();

    std::string serialize();
    static std::vector<ReplicaInfo> deserialize(std::string raw);

  private:
    int getNextId();
};

#endif // __ReplicaVector_hpp__