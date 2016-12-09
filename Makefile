CC_FLAGS =
INSTALL_DIR = /usr/local

all: static dynamic

dynamic: bin/libcex.so include/cex.h

static: bin/libcex.a include/cex.h

clean:
	rm -f src/cex.o*
	rm -rf include
	rm -rf bin
	rm -f test/test.o

install:
	if [ -f bin/libcex.a ]; then \
		install -m 644 -T bin/libcex.a $(INSTALL_DIR)/lib/libcex.a.0; \
		rm -f $(INSTALL_DIR)/lib/libcex.a; \
		ln $(INSTALL_DIR)/lib/libcex.a.0 $(INSTALL_DIR)/lib/libcex.a; \
	fi
	if [ -f bin/libcex.so ]; then \
		install -m 644 -T bin/libcex.so $(INSTALL_DIR)/lib/libcex.so.0; \
		rm -f $(INSTALL_DIR)/lib/libcex.so; \
		ln $(INSTALL_DIR)/lib/libcex.so.0 $(INSTALL_DIR)/lib/libcex.so; \
	fi
	cp -r include/cex.h $(INSTALL_DIR)/include
	ldconfig

test: bin/test
	bin/test

bin:
	mkdir bin

bin/libcex.a: bin src/cex.os
	ar rcs bin/libcex.a src/cex.os

bin/libcex.so: bin src/cex.od
	gcc $(CC_FLAGS) -shared -o bin/libcex.so src/cex.od

bin/test: bin include/cex.h test/test.c bin/libcex.a
	gcc $(CC_FLAGS) -o bin/test -I ./include test/test.c bin/libcex.a

include/cex.h: src/cex.h
	mkdir -p include/
	cp src/cex.h include/

src/cex.od: src/cex.c
	gcc $(CC_FLAGS) -c -fPIC -o src/cex.od src/cex.c

src/cex.os: src/cex.c
	gcc $(CC_FLAGS) -c -o src/cex.os src/cex.c

test/test.o:
	gcc $(CC_FLAGS) -c -o test/test.o test/test.c
