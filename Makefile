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

.PHONY: all clean

all : $(OBJ_FILES)

%: %.c
	$(CC) -o $@ $^

clean:
	-rm -f $(OBJ_FILES)
