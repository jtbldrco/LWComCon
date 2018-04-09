PROJECT_ROOT = $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

LIB_NAME = imsgsockhdlr
FULL_LIB_NAME_A = lib$(LIB_NAME).a
FULL_LIB_NAME_SO = lib$(LIB_NAME).so

MSG_HDLR_LIB_DIR = ../iway_msg_hdlr

PROG1 = MsgCommHdlrTestSender
PROG2 = MsgCommHdlrTestReceiver

PROGRAMNAMES = $(PROG1) $(PROG2)

## TODO - create library of ThreadedWorker and ThreadMapper, then access that
## from here; add include paths to pick up the headers for those classes

PROG1_OBJS = MsgCommHdlrTestSender.o MsgCommHdlr.o ThreadedWorker.o ThreadMapper.o
PROG2_OBJS = MsgCommHdlrTestReceiver.o MsgCommHdlr.o ThreadedWorker.o ThreadMapper.o

# Compilers
CC = gcc
CXX = g++


# Compiler Flags: C, C++, and Preprocessor
CFLAGS += -g -O0
CFLAGS += -pthread \
          -I../iway_msg_hdlr \
          -I../iway_logger \
          -I./ \
          -I/usr/local/include 

CXXFLAGS += -std=c++0x

# OPTIONAL - enable debugging to track internal execution flow
##### CPPFLAGS += -DDEBUG -DDEBUG_THREADEDWORKER -DDEBUG_MSGCOMMHDLR
CPPFLAGS += -DDEBUG -DDEBUG_THREADEDWORKER -DDEBUG_MSGCOMMHDLR

CPPFLAGS += -DDEBUG_THREADSAFEMSGPTRQUEUE

# NOTE the WARNING ...
CPPFLAGS += -DDEBUG_THREADEDWORKERTESTER_WARNING___ADDS_LONG_SLEEPS_IN_MSG_QUEUE


# Linker/Loader Flags
PN_LDFLAGS = -L/usr/lib/x86_64-linux-gnu \
             -l pthread \
             -L/usr/local/lib/x86_64-linux-gnu \
             -L$(MSG_HDLR_LIB_DIR) \
             -Wl,-rpath,$(MSG_HDLR_LIB_DIR) \
             -l $(LIB_NAME)

all:	$(PROGRAMNAMES)

$(PROG1):	$(PROG1_OBJS)
	$(CXX) -o $@ $^ $(PN_LDFLAGS)

$(PROG2):	$(PROG2_OBJS)
	$(CXX) -o $@ $^ $(PN_LDFLAGS)

%.o:	$(PROJECT_ROOT)%.cpp $(PROJECT_ROOT)%.h Makefile
	$(CXX) -c $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) $< -o $@

%.o:	$(PROJECT_ROOT)%.c $(PROJECT_ROOT)%.h Makefile
	$(CXX) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

clean:
	rm -fr $(PROGRAMNAMES) $(PROG1_OBJS) $(PROG2_OBJS)

