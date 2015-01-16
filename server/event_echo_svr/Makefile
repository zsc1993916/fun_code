CC = gcc
CXX = g++ 

CCFLAGS = -g -o

event_echo_svr : event_echo_svr.cpp
	$(CXX) $(CCFLAGS) $@ $^

event_echo_cli : event_echo_cli.cpp
	$(CXX) $(CCFLAGS) $@ $^

all: event_echo_svr event_echo_cli
	@echo "complie done!"

make clean :
	rm -rf event_echo_svr event_echo_cli
