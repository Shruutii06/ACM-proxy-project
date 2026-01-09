#include "../include/filter.h"
#include <fstream>
#include <algorithm>

static std::string to_lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

Filter::Filter(const std::string& filename) {
    std::ifstream file(filename);
    std::string domain;

    while (file >> domain) {
        blocked_domains.push_back(to_lower(domain));
    }
}

bool Filter::is_blocked(const std::string& host) const {
    std::string h = to_lower(host);

    for (const auto& blocked : blocked_domains) {
        if (h == blocked)
            return true;

        if (h.size() > blocked.size() &&
            h.compare(h.size() - blocked.size(), blocked.size(), blocked) == 0 &&
            h[h.size() - blocked.size() - 1] == '.')
            return true;
    }
    return false;
}


