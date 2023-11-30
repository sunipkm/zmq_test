CC=gcc
CXX=g++
EDCFLAGS= -O2 -I./ `pkg-config libczmq --cflags` $(CFLAGS)
EDCXXFLAGS= -O2 -std=c++17 -I./ `pkg-config libczmq --cflags` $(CXXFLAGS)
EDLDFLAGS= `pkg-config libczmq --libs` `pkg-config libzmq --libs` `pkg-config libsodium --libs` $(LDFLAGS)

SRCCFILES=$(wildcard *.c)
DESTCFILES=$(patsubst %.c,%.out,$(SRCCFILES))
SRCCPPFILES=$(wildcard *.cpp)
DESTCPPFILES=$(patsubst %.cpp,%.out,$(SRCCPPFILES))

all: $(DESTCFILES) $(DESTCPPFILES)

%.out: %.c
	$(CC) $(EDCFLAGS) -o $@ $< $(EDLDFLAGS)

%.out: %.cpp
	$(CXX) $(EDCXXFLAGS) -o $@ $< $(EDLDFLAGS)

clean:
	rm -f *.out