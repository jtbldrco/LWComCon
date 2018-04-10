PROJECT_ROOT = $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

LIB_NAME = ithreading
FULL_LIB_NAME_A = lib$(LIB_NAME).a
FULL_LIB_NAME_SO = lib$(LIB_NAME).so

LIB_NAMES = $(FULL_LIB_NAME_A) $(FULL_LIB_NAME_SO)

PROG1 = ThreadSafeMsgPtrQueueTest
PROG2 = ThreadSafePtrQueueTest

BUILD_PRODUCTS =  $(FULL_LIB_NAME_A) $(FULL_LIB_NAME_SO) $(PROG1) $(PROG2)

PROG1_OBJS = ThreadSafeMsgPtrQueueTest.o ThreadMapper.o ThreadedWorker.o ThreadedWorkerTester.o
PROG2_OBJS = ThreadSafePtrQueueTest.o ThreadMapper.o ThreadedWorker.o ThreadedWorkerTester.o

LIB_OBJS =  ThreadedWorker.o ThreadMapper.o

LIB_DIR = .

# Compilers
CC = gcc
CXX = g++


# Compiler Flags: C, C++, and Preprocessor
CFLAGS += -pthread -I/usr/local/include
CFLAGS += -g -O0
CFLAGS += -fPIC
CXXFLAGS += -std=c++0x

# OPTIONAL - enable debugging to track internal execution flow
CPPFLAGS += -DDEBUG -DDEBUG_THREADEDWORKER

# NOTE the WARNING ...
CPPFLAGS += -DDEBUG_THREADEDWORKERTESTER_WARNING___ADDS_LONG_SLEEPS_IN_MSG_QUEUE

# Linker/Loader Flags
PN_LDFLAGS = -L$(LIB_DIR) -L/usr/lib/x86_64-linux-gnu -l pthread -L/usr/local/lib/x86_64-linux-gnu

all:	$(BUILD_PRODUCTS)

install:	$(LIB_NAMES)
	cp $(FULL_LIB_NAME_A) /usr/lib
	cp $(FULL_LIB_NAME_SO) /usr/lib

$(FULL_LIB_NAME_A):	$(LIB_OBJS)
	ar -cvq -o $(LIB_DIR)/$@ $<

$(FULL_LIB_NAME_SO):	$(LIB_OBJS)
	$(CC) -shared -o $(LIB_DIR)/$@ $<

$(PROG1):	$(PROG1_OBJS)
	$(CXX) -o $@ $^ $(PN_LDFLAGS)

$(PROG2):	$(PROG2_OBJS)
	$(CXX) -o $@ $^ $(PN_LDFLAGS)

%.o:	$(PROJECT_ROOT)%.cpp
	$(CXX) -c $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

%.o:	$(PROJECT_ROOT)%.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<

clean:
	rm -fr $(BUILD_PRODUCTS) $(PROG1_OBJS) $(PROG2_OBJS) $(LIB_OBJS)

