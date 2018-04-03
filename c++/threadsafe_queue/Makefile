PROJECT_ROOT = $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

PROG1 = ThreadSafeMsgPtrQueueTest

PROGRAMNAMES = $(PROG1)

PROG1_OBJS = ThreadSafeMsgPtrQueueTest.o ThreadMapper.o ThreadedWorker.o ThreadedWorkerTester.o

# Compilers
CC = gcc
CXX = g++


# Compiler Flags: C, C++, and Preprocessor
CFLAGS += -pthread -I/usr/local/include
CFLAGS += -g -O0
CXXFLAGS += -std=c++0x

# OPTIONAL - enable debugging to track internal execution flow
CPPFLAGS += -DDEBUG -DDEBUG_THREADEDWORKER

# NOTE the WARNING ...
CPPFLAGS += -DDEBUG_THREADEDWORKERTESTER_WARNING___ADDS_LONG_SLEEPS_IN_MSG_QUEUE

# Linker/Loader Flags
PN_LDFLAGS = -L/usr/lib/x86_64-linux-gnu -l pthread -L/usr/local/lib/x86_64-linux-gnu

all:	$(PROGRAMNAMES)

$(PROG1):	$(PROG1_OBJS)
	$(CXX) -o $@ $^ $(PN_LDFLAGS)

%.o:	$(PROJECT_ROOT)%.cpp
	$(CXX) -c $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

%.o:	$(PROJECT_ROOT)%.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<

clean:
	rm -fr $(PROGRAMNAMES) $(PROG1_OBJS)

