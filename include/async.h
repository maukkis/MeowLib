#ifndef _INCLUDE_ASYNC_H
#define _INCLUDE_ASYNC_H
#include <chrono>
#include <coroutine>
#include <exception>
#include <thread>

template<class T>
class MeowAsync {
public:
  ~MeowAsync(){
    if(handle) handle.destroy();
  }
  struct promise_type {
    T value;
    MeowAsync get_return_object() { 
      return MeowAsync{
        std::coroutine_handle<promise_type>::from_promise(*this)
      };
    }
    std::suspend_never initial_suspend() noexcept { return {}; }
    std::suspend_always final_suspend() noexcept { return {}; }
    void return_value(T&& meow) noexcept { 
      value = std::move(std::forward<T>(meow));
    }

    void unhandled_exception() {
      std::terminate();
    }
  };
  T get(){
    while(!handle.done()){
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
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
