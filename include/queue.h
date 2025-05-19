#ifndef _QUEUE_H
#define _QUEUE_H

#include <mutex>
#include <queue>

template<class T>
class ThreadSafeQueue {
public:
  void addToQueue(const T& stuff);
  bool empty();
  T pop();
private:
  std::mutex mtx;
  std::queue<T> arf;
};

#endif
