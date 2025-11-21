#ifndef _INCLUDE_ASYNC_H
#define _INCLUDE_ASYNC_H
#include <chrono>
#include <condition_variable>
#include <coroutine>
#include <exception>
#include <mutex>
#include <thread>

template<class T>
class MeowAsync {
public:
  ~MeowAsync(){
    if(handle) handle.destroy();
  }
  struct promise_type {
    T value;
    std::mutex mtx;
    std::condition_variable cv;
    MeowAsync get_return_object() { 
      return MeowAsync{
        std::coroutine_handle<promise_type>::from_promise(*this)
      };
    }
    std::suspend_never initial_suspend() noexcept { return {}; }
    std::suspend_always final_suspend() noexcept { return {}; }
    void return_value(T&& meow) noexcept { 
      std::unique_lock<std::mutex> lock(mtx);
      value = std::move(std::forward<T>(meow));
      lock.unlock();
      cv.notify_one();
    }

    void unhandled_exception() {
      std::terminate();
    }
  };
  /// @brief awaits till the value is available and returns it
  T await(){
    std::unique_lock<std::mutex> lock(handle.promise().mtx);
    handle.promise().cv.wait(lock, [this](){
      return handle.done();
    });
    return handle.promise().value;
  }

  bool done(){
    return handle.done();
  }
private:
  explicit MeowAsync(std::coroutine_handle<promise_type> h) : handle{h} {};
  std::coroutine_handle<promise_type> handle;

};

#endif
