PROJECT_ROOT = $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

PROG1 = simple_pc_funcs_test
PROG2 = simple_pc_funcs_test_rand_gen

PROGRAMNAMES = $(PROG1) $(PROG2)

PROG1_OBJS = simple_pc_funcs_test.o
PROG2_OBJS = simple_pc_funcs_test_rand_gen.o

# Compilers
CC = gcc
CXX = g++

# Compiler Flags: C, C++, and Preprocessor
CFLAGS += -g -O0

CXXFLAGS += -std=c++0x

### CPPFLAGS += -DDEBUG

# Linker/Loader Flags
### PN_LDFLAGS = -L/usr/lib/x86_64-linux-gnu -pthread -L/usr/local/lib/x86_64-linux-gnu

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

