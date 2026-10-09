CC ?= cc
CFLAGS += -std=c11 -D_DEFAULT_SOURCE -D_NETBSD_SOURCE -Wall -Wextra -Wpedantic -O2
OBJ = main.o options.o listing.o format.o sort.o

all: myls

myls: $(OBJ)
	$(CC) $(CFLAGS) -o myls $(OBJ)

main.o: main.c listing.h options.h
	$(CC) $(CFLAGS) -c main.c -o main.o

options.o: options.c options.h
	$(CC) $(CFLAGS) -c options.c -o options.o

listing.o: listing.c listing.h format.h sort.h options.h
	$(CC) $(CFLAGS) -c listing.c -o listing.o

format.o: format.c format.h listing.h options.h
	$(CC) $(CFLAGS) -c format.c -o format.o

sort.o: sort.c sort.h listing.h options.h
	$(CC) $(CFLAGS) -c sort.c -o sort.o

test: myls
	sh tests/smoke.sh

clean:
	rm -f $(OBJ) myls

.PHONY: all test clean
