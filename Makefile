VERSION=0.0.1
BINARY=libmeshparse.so

all:
	$(MAKE) -C src
	cp src/$(BINARY) .

clean:
	$(MAKE) -C src clean
	rm -f $(BINARY)

install: all
	cp $(BINARY) /usr/lib/
	mkdir -p /usr/include/meshparse
	cp -vr include/*.h /usr/include/meshparse/
	ldconfig

uninstall:
	rm -rf /usr/include/meshparse
	rm -f /usr/lib/$(BINARY)
	ldconfig
