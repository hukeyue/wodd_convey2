## GNU Make 3.81
## Copyright (C) 2006  Free Software Foundation, Inc.
## This is free software; see the source for copying conditions.
## There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A
## PARTICULAR PURPOSE.
##
## This program built for i386-apple-darwin11.3.0
CC ?= cc
SRC_FILES := $(wildcard *.c)
OBJ_FILES := $(SRC_FILES:.c=)
OBJ32_FILES := $(SRC_FILES:.c=32)
OBJ64_FILES := $(SRC_FILES:.c=64)

CXX ?= c++
CXX_SRC_FILES := $(wildcard *.cc)
CXX_OBJ_FILES := $(CXX_SRC_FILES:.cc=)
CXX_OBJ32_FILES := $(CXX_SRC_FILES:.cc=32)
CXX_OBJ64_FILES := $(CXX_SRC_FILES:.cc=64)

CFLAGS := -UNDEBUG -D_FORTIFY_SOURCE=2 -std=c99
CXXFLAGS :=  -UNDEBUG -D_FORTIFY_SOURCE=2 -std=c++20
TARGET ?= native
ifneq ($(findstring win32,$(TARGET)),)
	CFLAGS += -municode
endif

.PHONY: all clean obj32 obj64

all : $(OBJ_FILES) $(CXX_OBJ_FILES)

obj32: $(OBJ32_FILES) $(CXX_OBJ32_FILES)

obj64: $(OBJ64_FILES) $(CXX_OBJ64_FILES)

%: %.c
	$(CC) -o $@ $^ -march=native -O3 $(CFLAGS)

%32: %.c
	$(CC) -o $@ $^ -m32 -Os -s $(CFLAGS)

%64: %.c
	$(CC) -o $@ $^ -m64 -Os -s $(CFLAGS)

%: %.cc
	$(CXX) -o $@ $^ -march=native -O3 $(CXXFLAGS)

%32: %.cc
	$(CXX) -o $@ $^ -m32 -Os -s $(CXXFLAGS)

%64: %.cc
	$(CXX) -o $@ $^ -m64 -Os -s $(CXXFLAGS)

clean:
	-rm -f $(OBJ_FILES) $(OBJ32_FILES) $(OBJ64_FILES) $(CXX_OBJ_FILES) $(CXX_OBJ32_FILES) $(CXX_OBJ64_FILES)
