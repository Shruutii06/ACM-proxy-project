#ifndef FILTER_H
#define FILTER_H

#include <string>
#include <vector>

class Filter {
public:
    explicit Filter(const std::string& filename);
    bool is_blocked(const std::string& host) const;

private:
    std::vector<std::string> blocked_domains;
};

#endif

