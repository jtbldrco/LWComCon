PROJECT_ROOT = $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

TEST_PROG1 = msg_sock_hdlr_test_01

BUILD_PRODUCTS = $(TEST_PROG1)

TEST_PROG1_OBJS = msg_sock_hdlr_test_01.o

LIB_NAME_SO = libimsgsockhdlr.so

# Compilers
CC = gcc
CXX = g++


# Compiler Flags: C, C++, and Preprocessor
CFLAGS += -I../../iway_logger/
CFLAGS += -I../
CFLAGS += -g -O0 -fPIC

CXXFLAGS += -std=c++0x

MSH_LIB_DIR=$(ORIGIN)/../

# Optional compile flags for debugging -
##### CPPFLAGS += -DDEBUG -DDEBUG_MSH
CPPFLAGS += -DDEBUG -DDEBUG_MSH

# Enable test case: 15-sec sleep delay between
# client socket connect and socket send
# Can induce return code MSH_MESSAGE_RECV_TIMEOUT
##### CPPFLAGS += DEBUG_MSH_DELAY_CLIENT_SEND___WARNING_ADDS_WAITS_IN_SEND

# Linker/Loader Flags
PN_LDFLAGS = -L/usr/lib/x86_64-linux-gnu -L$(MSH_LIB_DIR)

all:	$(BUILD_PRODUCTS)

$(TEST_PROG1):	$(TEST_PROG1_OBJS)
	$(CXX) -o $@ $^ $(PN_LDFLAGS) -Wl,-rpath,$(MSH_LIB_DIR) -L$(MSH_LIB_DIR) -l $(LIB_NAME_SO)

%.o:	$(PROJECT_ROOT)%.cpp
	$(CXX) -c $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

%.o:	$(PROJECT_ROOT)%.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<

clean:
	rm -fr $(BUILD_PRODUCTS) $(TEST_PROG1_OBJS)

