#include "../include/logger.h"
#include <fstream>
#include <ctime>

void log_event(const std::string& message) {
    std::ofstream log("proxy.log", std::ios::app);
    std::time_t now = std::time(nullptr);
    log << std::ctime(&now) << " " << message << "\n";
}
