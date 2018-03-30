PROJECT_ROOT = $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

PROG1 = server
PROG2 = client

PROGRAMNAMES = $(PROG1) $(PROG2)

PROG1_OBJS = server.o
PROG2_OBJS = client.o

# Compilers
CC = gcc
CXX = g++

### /usr/local/* directories for rabbitmq-c inclusion

# Compiler Flags: C, C++, and Preprocessor
### CFLAGS += -g -O0 -pthread -I/usr/local/include
CFLAGS += -g -O0
CXXFLAGS += -std=c++0x
CPPFLAGS += -DDEBUG

# Linker/Loader Flags
### PN_LDFLAGS = -L/usr/lib/x86_64-linux-gnu -pthread -L/usr/local/lib/x86_64-linux-gnu
PN_LDFLAGS = -L/usr/lib/x86_64-linux-gnu

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

