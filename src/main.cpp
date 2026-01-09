#include "../include/proxy.h"
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <iostream>

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8888);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_fd, (sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 10);

    std::cout << "Proxy running on port 8888\n";

    // #include <thread>

while (true) {
    int client_socket = accept(server_fd, nullptr, nullptr);
    if (client_socket < 0) continue;

    std::thread(handle_client, client_socket).detach();
}


    close(server_fd);
    return 0;
}
