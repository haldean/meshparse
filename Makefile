VERSION=0.0.1
BINARY=meshparse.so

all:
	$(MAKE) -C src
	cp src/$(BINARY) .

clean:
	$(MAKE) -C src clean
	rm -f $(BINARY)
