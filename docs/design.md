

## 1 Overview

This project implements a **custom network proxy server** in C++ that supports HTTP request forwarding, HTTPS tunneling using the CONNECT method, domain-based access control, concurrent client handling, and request logging. The proxy acts as an intermediary between clients (web browsers or command-line tools) and destination web servers, enforcing administrative rules while transparently forwarding allowed traffic.

The design prioritizes **correctness, stability, and simplicity**, making it suitable for academic use and moderate workloads.

---

## 2 High-Level Architecture

### Architecture Description

The system follows a **client–proxy–server** architecture. Clients are configured to route their network traffic through the proxy server. The proxy inspects incoming requests, applies filtering policies, and forwards valid requests to the target servers. Responses are then relayed back to the clients.

### Architecture Diagram 

The overall architecture of the proxy server is illustrated in **diagram.png**. The diagram shows the interaction between the client, the proxy server’s internal modules, and the destination servers.

### Internal Components

* **Main Server Module (main.cpp)**
  Initializes the listening socket, accepts incoming client connections, and spawns a new thread for each connection.

* **Proxy Handler (proxy.cpp)**
  Implements core proxy functionality including request parsing, domain filtering, HTTP forwarding, HTTPS tunneling, socket timeouts, and graceful connection termination.

* **HTTP Parser (http_parser.cpp)**
  Parses incoming HTTP requests to extract method, destination host, port number, and raw request data.

* **Domain Filter (filter.cpp)**
  Loads blocked domains from a configuration file and applies subdomain-aware matching to determine whether a request should be blocked.

* **Logger (logger.cpp)**
  Records proxy activity such as allowed requests, blocked requests, and HTTPS tunnel creation for auditing and debugging.

---

## 3 Concurrency Model and Rationale

### Concurrency Model Used

The proxy server uses a **thread-per-connection** concurrency model.

Each incoming client connection is handled by a separate thread created at the time of connection acceptance.

### Rationale

* Simplifies design and implementation
* Each client request is isolated, preventing blocking across clients
* Suitable for moderate numbers of concurrent clients
* Aligns well with the educational objectives of the project

Alternative models such as event-driven I/O or thread pools were considered but not implemented due to increased complexity and reduced clarity for this project’s scope.

---

## 4 Data Flow Description

### HTTP Request Handling Flow

1. Client sends an HTTP request to the proxy.
2. Proxy reads the request and parses HTTP headers.
3. Domain filter checks whether the destination host is blocked.
4. If blocked, the proxy returns an HTTP 403 Forbidden response.
5. If allowed:

   * Proxy establishes a TCP connection to the destination server.
   * The request is forwarded (using HTTP/1.0 semantics).
   * The response is read with socket timeouts.
   * The response is forwarded back to the client.
6. Connections are closed gracefully after completion.

### HTTPS Request Handling Flow (CONNECT Method)

1. Client sends a CONNECT request specifying target host and port.
2. Proxy validates the destination against domain filtering rules.
3. Proxy establishes a TCP connection to the destination server.
4. Proxy responds with `200 Connection Established`.
5. A bidirectional tunnel is created between client and server.
6. Encrypted traffic is forwarded without inspection until connection termination.

---

## 5 Error Handling

The proxy server includes multiple safeguards to ensure stability:

* Socket receive timeouts prevent indefinite blocking
* Half-close (`shutdown()`) semantics ensure proper connection termination
* DNS resolution failures are handled gracefully
* Malformed or unsupported requests are rejected safely
* All sockets are properly closed to prevent resource leaks

---

## 6 Limitations

* Full HTTP/1.1 features (persistent connections, chunked encoding) are not supported
* HTTPS traffic is not decrypted or inspected
* No authentication mechanism is implemented
* No caching for HTTPS responses

These limitations are acceptable given the scope and objectives of the project.

---

## 7 Security Considerations

* HTTPS traffic remains end-to-end encrypted, preserving confidentiality
* The proxy does not log request payloads or sensitive data
* Domain filtering reduces access to restricted or unsafe domains
* The server avoids indefinite blocking and denial-of-service risks via timeouts

---

## 8 Summary

The proxy server design achieves a balance between functionality, simplicity, and robustness. By combining modular architecture, concurrency support, and defensive networking practices, the system fulfills all project requirements while remaining extensible for future enhancements such as caching, authentication, or traffic analysis.

