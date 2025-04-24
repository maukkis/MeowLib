#include "includes/queue.h"
#include <mutex>

template<class T>
void ThreadSafeQueue<T>::addToQueue(const T& a){
  std::unique_lock<std::mutex> lock(mtx);
  arf.emplace(a);
}

template<class T>
bool ThreadSafeQueue<T>::empty(){
  std::unique_lock<std::mutex> lock(mtx);
  return arf.empty();
}

template<class T>
T ThreadSafeQueue<T>::pop(){
  std::unique_lock<std::mutex> lock(mtx);
  T val = std::move(arf.front());
  arf.pop();
  return val;
}
