## 1. Overview

This project implements a **custom network proxy server** in C++ that supports HTTP request forwarding, HTTPS tunneling via the CONNECT method, domain-based access control, concurrent client handling, and structured request logging.

The proxy acts as an intermediary between clients (web browsers or command-line tools such as `curl`) and destination web servers. Incoming client requests are inspected, validated against administrative policies, and either forwarded transparently or blocked based on configured rules.

The design prioritizes **correctness, robustness, modularity, and simplicity**, making the implementation suitable for academic evaluation and moderate real-world workloads while remaining extensible for future enhancements.

---

## 2. High-Level Architecture

### 2.1 Architecture Description

The system follows a classic **client–proxy–server** architecture:

1. Clients are explicitly configured to use the proxy server.
2. All HTTP/HTTPS traffic flows through the proxy.
3. The proxy inspects request metadata (method, host, port).
4. Administrative policies (domain filtering) are applied.
5. Allowed traffic is forwarded to destination servers.
6. Responses are relayed back to clients.

The proxy does **not modify application payloads** and does **not decrypt HTTPS traffic**, preserving end-to-end security.

---

### 2.2 Architecture Diagram

The overall architecture is illustrated in **`diagram.png`**, showing:

* Clients (browser / curl)
* Proxy server and its internal modules
* Destination web servers
* Bidirectional data flow for HTTP and HTTPS

---

### 2.3 Internal Components

#### **Main Server Module (`main.cpp`)**

* Initializes the listening socket
* Binds to the configured port
* Accepts incoming client connections
* Spawns a new thread for each connection
* Acts as the lifecycle controller for the proxy

---

#### **Proxy Handler (`proxy.cpp`)**

Implements the core proxy logic:

* Reads client requests
* Delegates parsing to the HTTP parser
* Applies domain filtering rules
* Handles HTTP forwarding
* Implements HTTPS tunneling via CONNECT
* Enforces socket timeouts
* Ensures graceful shutdown of connections

---

#### **HTTP Parser (`http_parser.cpp`)**

* Parses raw HTTP request data
* Extracts:

  * HTTP method
  * Target host
  * Target port
  * Raw request payload
* Supports both absolute-form requests and CONNECT requests

---

#### **Domain Filter (`filter.cpp`)**

* Loads blocked domains from `config/blocked_domains.txt`
* Performs **subdomain-aware matching**

  * Blocking `example.com` automatically blocks `sub.example.com`
* Ensures policy enforcement before outbound connections are made

---

#### **Logger (`logger.cpp`)**

* Records proxy activity to `logs/proxy.log`
* Logs:

  * Allowed requests
  * Blocked requests
  * HTTPS tunnel establishment
* Provides traceability for debugging and testing

---

## 3. Concurrency Model and Rationale

### 3.1 Concurrency Model Used

The proxy server uses a **thread-per-connection** concurrency model.

Each client connection is handled by a dedicated thread created immediately after connection acceptance.

---

### 3.2 Rationale

This model was selected because:

* It provides **strong isolation** between clients
* Blocking I/O in one connection does not affect others
* Implementation is straightforward and easy to reason about
* Debugging and correctness verification are simpler
* Suitable for moderate concurrency levels expected in academic settings

Alternative designs such as event-driven I/O or thread pools were considered but not implemented due to increased complexity and reduced transparency for this project’s scope.

---

## 4. Data Flow Description

### 4.1 HTTP Request Handling Flow

1. Client sends an HTTP request to the proxy.
2. Proxy reads the request from the client socket.
3. HTTP parser extracts method, host, and port.
4. Domain filter checks whether the host is blocked.
5. If blocked:

   * Proxy returns `HTTP/1.1 403 Forbidden`
   * Request is logged and connection closed
6. If allowed:

   * Proxy resolves the destination hostname
   * Establishes a TCP connection to the server
   * Forwards the HTTP request using HTTP/1.0 semantics
   * Reads the server response with timeouts
   * Forwards the response to the client
7. Both connections are closed gracefully

---

### 4.2 HTTPS Request Handling Flow (CONNECT Method)

1. Client sends a CONNECT request specifying host and port.
2. Proxy parses the request and applies domain filtering.
3. Proxy establishes a TCP connection to the destination server.
4. Proxy responds with:

   ```
   HTTP/1.1 200 Connection Established
   ```
5. A bidirectional tunnel is created between client and server.
6. Encrypted data is forwarded transparently using `select()`.
7. Tunnel closes on timeout, client disconnect, or server disconnect.

The proxy does **not inspect or modify HTTPS payloads**.

---

## 5. Error Handling

The proxy includes multiple defensive mechanisms:

* Receive and send timeouts to prevent indefinite blocking
* Graceful handling of DNS resolution failures
* Safe rejection of malformed or unsupported requests
* Proper socket shutdown and closure to prevent leaks
* Timeout-based termination of inactive HTTPS tunnels

These measures ensure stability even under malformed input or network failures.

---

## 6. Configuration Design

The proxy externalizes its behavior using configuration files located in the `config/` directory.

* **`blocked_domains.txt`**

  * Fully implemented
  * Controls domain-based access restrictions
  * Supports subdomain-aware blocking

* **`proxy.conf` and `logging.conf`**

  * Define extensible parameters such as ports, timeouts, and logging behavior
  * Provide a foundation for future enhancements
  * Not fully parsed in the current implementation, but documented for extensibility

This design avoids recompilation for policy changes and improves maintainability.

---

## 7. Limitations

* HTTP/1.1 persistent connections are not supported
* Chunked transfer encoding is not handled
* HTTPS traffic is not decrypted or cached
* No user authentication or access control
* No rate limiting or bandwidth shaping

These limitations are acceptable given the project scope and learning objectives.

---

## 8. Security Considerations

* HTTPS traffic remains end-to-end encrypted
* The proxy does not log request bodies or credentials
* Domain filtering reduces exposure to restricted domains
* Socket timeouts reduce denial-of-service risks
* No sensitive data is stored or processed by the proxy

---

## 9. Summary

This proxy server demonstrates a complete, modular, and robust implementation of an HTTP/HTTPS forwarding proxy. Through clear architectural separation, explicit concurrency design, defensive networking practices, and configurable policy enforcement, the system fulfills all project requirements while remaining extensible for future work such as caching, authentication, or traffic analysis.

---

