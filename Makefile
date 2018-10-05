CC = gcc
CFLAGS = -O3 -D_FORTIFY_SOURCE=2 -D_POSIX_SOURCE -Wall -Werror -pedantic -std=c11 -D_GNU_SOURCE -lmagic

SRCDIR = src
HOMEDIR = public_html

BINARY = server

PORT = 8088

default: all

all: build

run:
	./$(BINARY) $(PORT) $(HOMEDIR)

clean:
	rm -rf $(BINARY)

.PHONY: default all run clean

build:
	$(CC) $(SRCDIR)/*.{c,h} -o $(BINARY) $(CFLAGS)
