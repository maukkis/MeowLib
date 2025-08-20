#ifndef _QUEUE_H
#define _QUEUE_H

#include <mutex>
#include <queue>

template<class T>
class ThreadSafeQueue {
public:
  void addToQueue(const T& a){
    std::unique_lock<std::mutex> lock(mtx);
    arf.emplace(a);
  }
  bool empty(){
    return arf.empty();
  }
  T pop(){
    std::unique_lock<std::mutex> lock(mtx);
    T val = std::move(arf.front());
    arf.pop();
    return val;
  }
private:
  std::mutex mtx;
  std::queue<T> arf;
};

#endif
