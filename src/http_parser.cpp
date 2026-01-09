#include "../include/http_parser.h"
// #include "http_parser.h"
#include <sstream>

bool parse_http_request(const std::string& data, HttpRequest& req) {
    req.raw = data;
    std::istringstream stream(data);
    std::string line;

    // Request line
    if (!std::getline(stream, line)) return false;
    std::istringstream rl(line);
    rl >> req.method >> req.uri >> req.version;

    // 🔑 CONNECT parsing (CRITICAL FIX)
    if (req.method == "CONNECT") {
        size_t colon = req.uri.find(':');
        if (colon == std::string::npos) return false;

        req.host = req.uri.substr(0, colon);
        req.port = std::stoi(req.uri.substr(colon + 1));
        return true;
    }

    // Normal HTTP parsing
    while (std::getline(stream, line) && line != "\r") {
        if (line.find("Host:") == 0) {
            std::string host = line.substr(5);
            host.erase(0, host.find_first_not_of(" \t"));
            host.erase(host.find_last_not_of("\r") + 1);

            size_t colon = host.find(':');
            if (colon != std::string::npos) {
                req.host = host.substr(0, colon);
                req.port = std::stoi(host.substr(colon + 1));
            } else {
                req.host = host;
                req.port = 80;
            }
        }
    }

    return !req.host.empty();
}
