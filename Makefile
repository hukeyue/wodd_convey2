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
CFLAGS := -UNDEBUG -D_FORTIFY_SOURCE=2
TARGET ?= native
ifneq ($(findstring win32,$(TARGET)),)
	CFLAGS += -municode
endif

.PHONY: all clean obj32 obj64

all : $(OBJ_FILES)

obj32: $(OBJ32_FILES)

obj64: $(OBJ64_FILES)

%: %.c
	$(CC) -o $@ $^ -march=native -O3 $(CFLAGS)

%32: %.c
	$(CC) -o $@ $^ -m32 -Os -s $(CFLAGS)

%64: %.c
	$(CC) -o $@ $^ -m64 -Os -s $(CFLAGS)

clean:
	-rm -f $(OBJ_FILES) $(OBJ32_FILES) $(OBJ64_FILES)
