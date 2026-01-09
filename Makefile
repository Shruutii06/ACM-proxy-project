CXX = g++
CXXFLAGS = -Wall -pthread

SRC = src/main.cpp src/proxy.cpp src/http_parser.cpp src/filter.cpp src/logger.cpp src/cache.cpp
OUT = proxy

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT)

clean:
	rm -f proxy
