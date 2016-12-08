CC_FLAGS =
INSTALL_DIR = /usr/local/lib

dynamic: bin/libcex.so

static: bin/libcex.a

clean:
	rm src/cex.o*

install:
	if [ -f bin/libcex.a ]; then \
		install -m 644 -T bin/libcex.a $(INSTALL_DIR)/libcex.a.0; \
		rm -f $(INSTALL_DIR)/libcex.a; \
		ln $(INSTALL_DIR)/libcex.a.0 $(INSTALL_DIR)/libcex.a; \
	fi
	if [ -f bin/libcex.so ]; then \
		install -m 644 -T bin/libcex.so $(INSTALL_DIR)/libcex.so.0; \
		rm -f $(INSTALL_DIR)/libcex.so; \
		ln $(INSTALL_DIR)/libcex.so.0 $(INSTALL_DIR)/libcex.so; \
	fi
	ldconfig

bin:
	mkdir bin

bin/libcex.a: bin src/cex.os
	ar rcs bin/libcex.a src/cex.os

bin/libcex.so: bin src/cex.od
	gcc $(CC_FLAGS) -shared -o bin/libcex.so src/cex.od

src/cex.od: src/cex.c
	gcc $(CC_FLAGS) -c -fPIC -o src/cex.od src/cex.c

src/cex.os: src/cex.c
	gcc $(CC_FLAGS) -c -o src/cex.os src/cex.c
