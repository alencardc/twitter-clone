#ifndef __vector_pair_hpp__
#define __vector_pair_hpp__

#include <vector>

template<class T1, class T2>
void extract_first(const std::vector<std::pair<T1, T2> >& v, std::vector<T1>& v1) {
  v1.resize(v.size());
  for (size_t i = 0; i < v.size(); i++) {
    v1[i] = v[i].first;
  }
}

template<class T1, class T2>
void extract_second(const std::vector<std::pair<T1, T2> >& v, std::vector<T2>& v2) {
  v2.resize(v.size());
  for (size_t i = 0; i < v.size(); i++) {
    v2[i] = v[i].second;
  }
}

#endif //  __vector_pair_hpp__