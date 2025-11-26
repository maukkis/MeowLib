#ifndef _INCLUDE_LRUCACHE_H
#define _INCLUDE_LRUCACHE_H
#include <unordered_map>
#include <memory>
#include <optional>
#include <expected>
#include <utility>

template<class K, class T>
struct Node {
  Node *next = nullptr;
  Node *prev = nullptr;
  T val;
  K key;
};



template<class T, class V>
class LruCache {
public:

  LruCache(int size){
    maxSize = size;
  }

  ~LruCache(){
    map.clear();
    freeCache();
  }

  void clear(){
    map.clear();
    freeCache();
  }
  
  void setMaxSize(size_t size){
    maxSize = size;
  }

  void insert(const T& key, const V& val){
    if(!hd){
      hd = new Node<T, V>;
      tl = hd;
      hd->val = val;
      hd->key = key;
      map.insert({key, hd});
      return;
    }
    if(map.contains(key)){
      auto ptr = map[key];
      ptr->val = val;
      moveToHead(ptr);
      return;
    }
    if(map.size() >= maxSize){
      auto ptr = tl;
      map.erase(tl->key);
      tl->prev->next = nullptr;
      tl = tl->prev;
      delete ptr;
    }
    hd->prev = new Node<T, V>();
    hd->prev->next = hd;
    hd = hd->prev;
    hd->prev = nullptr;
    hd->val = val;
    hd->key = key;
    map.insert({key, hd});
  }


  std::optional<V> get(const T& key){
    if(!map.contains(key)) return std::nullopt;
    auto ptr = map[key];
    moveToHead(ptr);
    return ptr->val;
  }
  
  std::optional<V> operator[](const T& a){
    return get(a);
  }

  void erase(const T& key){
    if(!map.contains(key)) return;
    auto ptr = map[key];
    if(ptr == hd){
      hd = hd->next;
      if(hd)
        hd->prev = nullptr;
      delete ptr;
      map.erase(key);
      return;
    }
    if(ptr == tl){
      map.erase(key);
      tl->prev->next = nullptr;
      tl = tl->prev;
      delete ptr;
      return;
    }
    ptr->prev->next = ptr->next;
    ptr->next->prev = ptr->prev;
    map.erase(key);
    delete ptr;
  }


  class iterator {
    public:
      using value_type = std::pair<T, V>;
      using difference_type = std::ptrdiff_t;

      iterator() = default;

      iterator(Node<T, V> *hd){
        current = hd;
      }

      iterator(const iterator& a){
        current = a.current;
      }
      iterator& operator++(){
        current = current->next;
        return *this;
      }

      iterator operator++(int){
        auto tmp = iterator(current);
        current = current->next;
        return tmp;
      }

      bool operator==(const iterator& a) const{
        if(a.current == current) return true;
        return false;
      }

      value_type operator*() const{
        return std::make_pair(current->key, current->val);
      }

      value_type operator->() const{
        return std::make_pair(current->key, current->val);
      }

      iterator operator--(int){
        auto tmp = iterator(current);
        current = current->prev;
        return tmp;
      }

      iterator& operator--(){
        current = current->prev;
        return *this;
      }

      iterator& operator=(const iterator& a){
        current = a.current;
        return *this;
      }
    private:
      Node<T, V> *current = nullptr;
  };


  iterator begin(){
    return iterator(hd);
  }

  iterator end(){
    return iterator(nullptr);
  }
private:
  void freeCache(){
    auto ptr = hd;
    while(ptr != nullptr){
      auto current = ptr;
      ptr = ptr->next;
      delete current;
    }
    hd = nullptr;
    tl = nullptr;
  }

  void moveToHead(Node<T, V> *ptr){
    if(ptr == hd) return;
    if(ptr == tl){
      tl = ptr->prev;
      tl->next = nullptr;
      hd->prev = ptr;
      ptr->next = hd;
      hd = ptr;
      ptr->prev = nullptr;
      return;
    }
    ptr->prev->next = ptr->next;
    ptr->next->prev = ptr->prev;
    hd->prev = ptr;
    ptr->next = hd;
    ptr->prev = nullptr;
    hd = ptr;
  }

  std::unordered_map<T, Node<T, V>*> map;
  Node<T, V> *hd = nullptr;
  Node<T, V> *tl = nullptr;
  size_t maxSize = 0;
};

static_assert(std::bidirectional_iterator<LruCache<int, int>::iterator>, "iterator not complete");

#endif
