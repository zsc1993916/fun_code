CC = gcc
CXX = g++ 

CCFLAGS = -g 

simple_echo_svr : simple_echo_svr.cpp
	$(CXX) $(CCFLAGS) $^ -o $@

simple_echo_cli : simple_echo_cli.cpp
	$(CXX) $(CCFLAGS) $^ -o $@

.PHONY: all clean

all: simple_echo_svr simple_echo_cli
	@echo "compile done!"

clean :
	rm -rf simple_echo_svr simple_echo_cli
