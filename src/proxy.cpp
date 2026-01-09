#include "../include/proxy.h"
#include "../include/http_parser.h"
#include "../include/filter.h"
#include "../include/logger.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/time.h>

#include <cstring>
#include <string>
#include <iostream>

static Filter filter("config/blocked_domains.txt");

void set_timeout(int fd) {
    timeval tv{};
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
}

void handle_client(int client_fd) {
    char buffer[8192];

    ssize_t n = recv(client_fd, buffer, sizeof(buffer), 0);
    if (n <= 0) {
        close(client_fd);
        return;
    }

    std::string request(buffer, n);
    HttpRequest req;

    if (!parse_http_request(request, req)) {
        close(client_fd);
        return;
    }

    if (filter.is_blocked(req.host)) {
        std::string resp =
            "HTTP/1.1 403 Forbidden\r\n"
            "Content-Length: 0\r\n\r\n";
        send(client_fd, resp.c_str(), resp.size(), 0);
        log_event("BLOCKED " + req.host);
        close(client_fd);
        return;
    }

    hostent* server = gethostbyname(req.host.c_str());
    if (!server) {
        close(client_fd);
        return;
    }

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(req.port);
    memcpy(&addr.sin_addr, server->h_addr, server->h_length);

    if (connect(server_fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        close(client_fd);
        close(server_fd);
        return;
    }

    set_timeout(server_fd);
    set_timeout(client_fd);

    /* HTTPS CONNECT */
    if (req.method == "CONNECT") {
        std::string ok =
            "HTTP/1.1 200 Connection Established\r\n\r\n";
        send(client_fd, ok.c_str(), ok.size(), 0);

        while ((n = recv(client_fd, buffer, sizeof(buffer), 0)) > 0)
            send(server_fd, buffer, n, 0);

        close(server_fd);
        close(client_fd);
        return;
    }

    /* Force HTTP/1.0 + close */
    std::string forward = req.raw;
    size_t pos = forward.find("HTTP/1.1");
    if (pos != std::string::npos)
        forward.replace(pos, 8, "HTTP/1.0");

    forward += "\r\n";

    send(server_fd, forward.c_str(), forward.size(), 0);

    /* 🔑 CRITICAL LINE */
    shutdown(server_fd, SHUT_WR);

    /* Read until timeout or EOF */
    while ((n = recv(server_fd, buffer, sizeof(buffer), 0)) > 0) {
        send(client_fd, buffer, n, 0);
    }

    log_event("ALLOWED " + req.host);

    close(server_fd);
    close(client_fd);
}
