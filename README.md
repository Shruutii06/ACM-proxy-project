# Custom Network Proxy Server (C++)

## Project Overview

This project implements a **custom network proxy server** in C++ that supports:

* HTTP request forwarding
* HTTPS tunneling using the CONNECT method
* Smart, subdomain-aware domain filtering
* Concurrent handling of multiple clients
* Request logging for auditing and debugging

The proxy acts as an intermediary between clients (browsers or command-line tools like `curl`) and destination web servers, enforcing access control policies while transparently forwarding allowed traffic.

---

## Project Structure

```
proxy-project/
├── src/
│   ├── main.cpp            # Server setup and connection handling
│   ├── proxy.cpp           # Core proxy logic
│   ├── http_parser.cpp     # HTTP request parsing
│   ├── filter.cpp          # Domain filtering logic
│   └── logger.cpp          # Logging utility
├── include/
│   ├── proxy.h
│   ├── http_parser.h
│   ├── filter.h
│   └── logger.h
├── config/
│   └── blocked_domains.txt # List of blocked domains
├── logs/
│   └── proxy.log           # Runtime logs
├── tests/
│   ├── test_http.sh        # HTTP forwarding test
│   ├── test_blocking.sh    # Domain blocking test
│   ├── test_concurrency.sh # Concurrent clients test
│   └── test_malformed.sh   # Malformed request test
├── doccs/
│   ├── design.md           # Design document
│   └── diagram.png         # Architecture diagram
├── Makefile
└── README.md
```

---

## Features Implemented

* HTTP request forwarding (port 80)
* HTTPS tunneling using CONNECT (port 443)
* Subdomain-aware domain filtering
* Thread-per-connection concurrency model
* Graceful socket shutdown and timeout handling
* Logging of allowed, blocked, and tunneled requests
* Configurable blocked domain list

---
## Configuration

The proxy uses external configuration files located in the `config/` directory.

- `blocked_domains.txt`  
  Contains domains that should be blocked. Subdomains are automatically blocked.

- `proxy.conf`  
  Defines core proxy parameters such as listening port and timeouts.

- `logging.conf`  
  Controls logging behavior and log file location.

Only `blocked_domains.txt` is actively parsed by the current implementation; the remaining configuration files provide extensibility for future enhancements.

--- 

## Build Instructions

### Prerequisites

* Linux environment
* `g++` compiler
* `make`
* `curl` (for testing)
* `netcat` (`nc`) for malformed request testing

### Build the Project

From the project root directory:

```bash
make clean
make
```

This will generate the executable named `proxy`.

---

## Running the Proxy Server

Start the proxy server using:

```bash
./proxy
```

The proxy listens on **port 8888** by default.

---

## Configuring Clients to Use the Proxy

### Using curl

```bash
curl -x localhost:8888 http://example.com
curl -x localhost:8888 https://example.com
```

---

### Using a Web Browser (Chrome / Firefox)

1. Open browser network or proxy settings
2. Set **Manual Proxy Configuration**
3. HTTP Proxy: `localhost`
4. Port: `8888`
5. Enable proxy for both HTTP and HTTPS
6. Open any HTTP or HTTPS website

---

## Domain Blocking Configuration

Blocked domains are defined in:

```
config/blocked_domains.txt
```

Example:

```
bbc.co.uk
facebook.com
```

### Behavior

* Blocking a parent domain automatically blocks all its subdomains
* Example:

  * `bbc.co.uk`
  * `www.bbc.co.uk`
  * `news.bbc.co.uk`

All will be blocked.

Blocked requests return:

```
HTTP/1.1 403 Forbidden
```

---

## Logging

All proxy activity is logged to:

```
logs/proxy.log
```

### Sample Log Entries

```
ALLOWED example.com
BLOCKED bbc.co.uk
BLOCKED news.bbc.co.uk
HTTPS TUNNEL example.com
```

---

## Running Tests

All test scripts are located in the `tests/` directory.

### Make test scripts executable (only once)

```bash
chmod +x tests/*.sh
```

---

### Test 1: HTTP Forwarding

```bash
./tests/test_http.sh
```

Expected result:

* HTML content from `example.com`
* Log entry marked as `ALLOWED`

---

### Test 2: Domain Blocking

```bash
./tests/test_blocking.sh
```

Expected result:

* `403 Forbidden`
* Log entry marked as `BLOCKED`

---

### Test 3: Concurrent Clients

```bash
./tests/test_concurrency.sh
```

Expected result:

* Multiple requests handled simultaneously
* No hanging or crashes

---

### Test 4: Malformed Request Handling

```bash
./tests/test_malformed.sh
```

Expected result:

* Connection closed safely
* Proxy continues running normally

---

## Design Documentation

Detailed design explanations are available in:

```
doccs/design.md
```

Architecture diagram:

```
doccs/diagram.png
```

These documents describe:

* High-level architecture
* Concurrency model
* Data flow
* Error handling and limitations

---

## Limitations

* Full HTTP/1.1 features (persistent connections, chunked encoding) are not supported
* HTTPS traffic is not decrypted or inspected
* No proxy authentication mechanism
* No caching enabled

These limitations are acceptable given the project scope.

---

## Future Enhancements

* HTTP response caching
* Proxy authentication
* Thread pool or event-driven concurrency model
* Traffic statistics and monitoring dashboard

---

## Author

**Shruti**
B.Tech – Metallurgy and Materials Engineering
Indian Institute of Technology Roorkee


