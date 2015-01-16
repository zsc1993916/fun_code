CC = gcc
CXX = g++ 

CCFLAGS = -g 

direct_svr : direct_svr.cpp
	$(CXX) $(CCFLAGS) $^ -o $@

client : client.cpp
	$(CXX) $(CCFLAGS) $^ -o $@

.PHONY: all clean

all: direct_svr client
	@echo "compile done!"

clean :
	rm -rf direct_svr client
