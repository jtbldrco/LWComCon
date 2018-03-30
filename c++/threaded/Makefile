PROJECT_ROOT = $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

PROG1 = ThreadMapperTest
PROG2 = ThreadedWorkerTest

## TODO - create library of ThreadedWorker and ThreadMapper, for access from
## project ../msg_comm_hdlr/
PROGRAMNAMES = $(PROG1) $(PROG2)

PROG1_OBJS = ThreadMapperTest.o ThreadMapper.o
PROG2_OBJS = ThreadedWorkerTest.o ThreadedWorker.o ThreadMapper.o ExampleThreadedWorker.o

# Compilers
CC = gcc
CXX = g++


# Compiler Flags: C, C++, and Preprocessor
CFLAGS += -pthread -I/usr/local/include
CFLAGS += -g -O0
CXXFLAGS += -std=c++0x

# OPTIONAL - enable debugging to track internal execution flow
# CPPFLAGS += -DDEBUG -DDEBUG_THREADEDWORKER

# Linker/Loader Flags
PN_LDFLAGS = -L/usr/lib/x86_64-linux-gnu -l pthread -L/usr/local/lib/x86_64-linux-gnu

all:	$(PROGRAMNAMES)

$(PROG1):	$(PROG1_OBJS)
	$(CXX) -o $@ $^ $(PN_LDFLAGS)

$(PROG2):	$(PROG2_OBJS)
	$(CXX) -o $@ $^ $(PN_LDFLAGS)

%.o:	$(PROJECT_ROOT)%.cpp
	$(CXX) -c $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

%.o:	$(PROJECT_ROOT)%.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<

clean:
	rm -fr $(PROGRAMNAMES) $(PROG1_OBJS) $(PROG2_OBJS)

