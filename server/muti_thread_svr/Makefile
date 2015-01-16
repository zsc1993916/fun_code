CC = gcc
CXX = g++ 

CCFLAGS = -g -o

muti_thread_echo_svr : muti_thread_echo_svr.cpp
	$(CXX) $(CCFLAGS) $@ $^ -I./ -lpthread

muti_thread_echo_cli : muti_thread_echo_cli.cpp
	$(CXX) $(CCFLAGS) $@ $^

all: muti_thread_echo_svr muti_thread_echo_cli
	@echo "complie done!"

clean :
	rm -rf muti_thread_echo_svr muti_thread_echo_cli
