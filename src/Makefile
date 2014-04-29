VERSION=0.0.1
BINARY=meshparse.so.$(VERSION)

CC=clang++
CFLAGS=-c -Wall -I../include/ -I../libs/ --std=c++0x -I/usr/include -O2 -fvisibility=hidden -fPIC
LDFLAGS=-L/usr/local/lib -lm -lglut -lGL -lGLU -lre2 -shared

SOURCES=$(wildcard *.cpp)
OBJECTS=$(SOURCES:.cpp=.o)

ifdef DEBUG
CFLAGS+=-O0 -g -DDEBUG
endif

all: $(SOURCES) $(BINARY)

$(BINARY): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(BINARY)
