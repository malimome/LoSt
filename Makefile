# Makefile for Shacham Waters private scheme of the Proof of Retrievability (POR).


RANLIB = ranlib
AR = ar


VERSION = 1.0
DIRNAME= rscode-$(VERSION)


CC = gcc
# OPTIMIZE_FLAGS = -O69
DEBUG_FLAGS = -O2
CFLAGS = $(DEBUG_FLAGS) -I include/
LDFLAGS = $(OPTIMIZE_FLAGS) $(DEBUG_FLAGS)

LIB_CSRC = rs.cpp galois.cpp berlekamp.cpp crcgen.cpp por.cpp client.cpp server.cpp publish.cpp clientUtils.cpp serverUtils.cpp socket.cpp utils.cpp
LIB_HSRC = ecc.h
LIB_OBJS = rs.o galois.o berlekamp.o crcgen.o por.o clientUtils.o socket.o utils.o

TARGET_LIB = libecc.a
TEST_PROGS = server client publish

TARGETS = $(TARGET_LIB) $(TEST_PROGS)

all: $(TARGETS)

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET_LIB): $(LIB_OBJS)
	$(RM) $@
	$(AR) cq $@ $(LIB_OBJS)
	if [ "$(RANLIB)" ]; then $(RANLIB) $@; fi

server: server.o socket.o serverUtils.o clientUtils.o por.o galois.o berlekamp.o crcgen.o rs.o
	$(CC) $(CFLAGS) -o pserver server.o serverUtils.o libecc.a libgmp.a libcrypto.a -ldl -lstdc++ -lrt

client: client.o socket.o clientUtils.o por.o galois.o berlekamp.o crcgen.o rs.o
	$(CC) $(CFLAGS) -o pclient client.o libecc.a libgmp.a libcrypto.a -lrt -ldl -lstdc++
	
publish: publish.o socket.o clientUtils.o por.o galois.o berlekamp.o crcgen.o rs.o
	$(CC) $(CFLAGS) -o publish publish.o libecc.a libgmp.a libcrypto.a -ldl -lstdc++ -lrt
	./copyFiles.sh
	
clean:
	rm -f *.o pserver pclient libecc.a
	rm -f *~

dist:
	(cd ..; tar -cvf rscode-$(VERSION).tar $(DIRNAME))

depend:
	makedepend $(SRCS)

# DO NOT DELETE THIS LINE -- make depend depends on it.

