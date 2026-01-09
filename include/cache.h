#pragma once
#include <unordered_map>
#include <list>
#include <string>

struct CacheEntry {
    std::string response;
};

class HttpCache {
public:
    HttpCache(size_t max_size = 10);

    bool get(const std::string& key, std::string& value);
    void put(const std::string& key, const std::string& value);

private:
    size_t capacity;
    std::list<std::string> lru;
    std::unordered_map<std::string,
        std::pair<CacheEntry, std::list<std::string>::iterator>> cache;
};
