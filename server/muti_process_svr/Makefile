CC = gcc
CXX = g++ 

CCFLAGS = -g -o

muti_process_echo_svr : muti_process_echo_svr.cpp
	$(CXX) $(CCFLAGS) $@ $^

muti_process_echo_cli : muti_process_echo_cli.cpp
	$(CXX) $(CCFLAGS) $@ $^

all: muti_process_echo_svr muti_process_echo_cli
	@echo "complie done!"

clean :
	rm -rf muti_process_echo_svr muti_process_echo_cli
