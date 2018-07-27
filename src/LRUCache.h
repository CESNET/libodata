#ifndef SRC_LRUCACHE_H_
#define SRC_LRUCACHE_H_

#include <boost/optional.hpp>
#include <cstdint>
#include <list>
#include <map>
#include <mutex>

namespace OData {

template <typename Key, typename Value> class LRUCache {
public:
  LRUCache(std::uint32_t size) : size(size), cache(), object_list(), mutex() {
  }
  ~LRUCache() = default;
  LRUCache(const LRUCache&) = delete;
  LRUCache& operator=(const LRUCache&) = delete;

  void put(const Key& key, Value value) {
    std::lock_guard<std::mutex> guard(mutex);
    auto it = cache.find(key);
    if (it == cache.end()) {
      if (cache.size() == size) {
        cache.erase(object_list.front());
        object_list.pop_front();
      }
      object_list.push_back(key);
      cache[key] = value;
    } else {
      it->second = value;
      updateLastAccessed(key);
    }
  }

  boost::optional<Value> get(const Key& key) const {
    std::lock_guard<std::mutex> guard(mutex);
    auto it = cache.find(key);
    if (it == cache.end()) {
      return boost::optional<Value>();
    } else {
      updateLastAccessed(key);
      return boost::optional<Value>(it->second);
    }
  }

  void remove(const Key& key) {
    std::lock_guard<std::mutex> guard(mutex);
    auto it = cache.find(key);
    if (it != cache.end()) {
      auto key_index = std::find(object_list.begin(), object_list.end(), key);
      object_list.erase(key_index);
      cache.erase(it);
    }
  }

private:
  void updateLastAccessed(const Key& key) const {
    auto key_index = std::find(object_list.begin(), object_list.end(), key);
    object_list.erase(key_index);
    object_list.push_back(key);
  }

  const std::uint32_t size;
  std::map<Key, Value> cache;
  mutable std::list<Key> object_list;
  mutable std::mutex mutex;
};

} /* namespace OData */

#endif /* SRC_LRUCACHE_H_ */
