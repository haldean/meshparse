VERSION=0.0.1
BINARY=libmeshparse.so

CFLAGS=-c -pedantic -Wall -I../include/ -I../libs/ --std=c++11 -I/usr/include -O2 -fPIC
LDFLAGS=-L/usr/local/lib -lm -lre2 -shared

SOURCES=$(wildcard *.cpp)
OBJECTS=$(SOURCES:.cpp=.o)

ifdef DEBUG
CFLAGS+=-O0 -g -DDEBUG
endif

all: $(SOURCES) $(BINARY)

$(BINARY): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CXX) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(BINARY)
