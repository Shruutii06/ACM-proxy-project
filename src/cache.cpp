#include "../include/cache.h"

HttpCache::HttpCache(size_t max_size) : capacity(max_size) {}

bool HttpCache::get(const std::string& key, std::string& value) {
    auto it = cache.find(key);
    if (it == cache.end()) return false;

    lru.erase(it->second.second);
    lru.push_front(key);
    it->second.second = lru.begin();

    value = it->second.first.response;
    return true;
}

void HttpCache::put(const std::string& key, const std::string& value) {
    if (cache.find(key) != cache.end()) return;

    if (cache.size() >= capacity) {
        std::string old = lru.back();
        lru.pop_back();
        cache.erase(old);
    }

    lru.push_front(key);
    cache[key] = { CacheEntry{value}, lru.begin() };
}
