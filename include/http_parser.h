#pragma once
#include <string>
#include <unordered_map>

struct HttpRequest {
    std::string method;
    std::string uri;
    std::string version;
    std::string host;
    int port = 80;
    std::string raw;
};

bool parse_http_request(const std::string& data, HttpRequest& req);
