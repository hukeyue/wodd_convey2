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

.PHONY: all clean

all : $(OBJ_FILES) $(OBJ32_FILES)

%: %.c
	$(CC) -o $@ $^

%32: %.c
	$(CC) -o $@ $^ -m32

clean:
	-rm -f $(OBJ_FILES) $(OBJ32_FILES)
