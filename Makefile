CC=gcc
CXX=g++
EDCFLAGS= -O2 -I./ `pkg-config libczmq --cflags` $(CFLAGS)
EDLDFLAGS= `pkg-config libczmq --libs` `pkg-config libzmq --libs` `pkg-config libsodium --libs` $(LDFLAGS)

SRCFILES=$(wildcard *.c)
DESTFILES=$(patsubst %.c,%.out,$(SRCFILES))

all: $(DESTFILES)

%.out: %.c
	$(CC) $(EDCFLAGS) -o $@ $< $(EDLDFLAGS)