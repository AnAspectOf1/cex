CC_FLAGS =
INSTALL_DIR = /usr/local

dynamic: bin/libcex.so include/cex/cex.h

static: bin/libcex.a include/cex/cex.h

clean:
	rm src/cex.o*

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
	cp -r include/* $(INSTALL_DIR)/include
	ldconfig

bin:
	mkdir bin

bin/libcex.a: bin src/cex.os
	ar rcs bin/libcex.a src/cex.os

bin/libcex.so: bin src/cex.od
	gcc $(CC_FLAGS) -shared -o bin/libcex.so src/cex.od

include/cex/cex.h: src/cex.h
	mkdir -p include/cex
	cp src/cex.h include/cex

src/cex.od: src/cex.c
	gcc $(CC_FLAGS) -c -fPIC -o src/cex.od src/cex.c

src/cex.os: src/cex.c
	gcc $(CC_FLAGS) -c -o src/cex.os src/cex.c
