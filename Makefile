CC ?= cc
CFLAGS ?= -std=c11 -D_DEFAULT_SOURCE -Wall -Wextra -Wpedantic -O2
CPPFLAGS ?= -MMD -MP
OBJ = main.o options.o listing.o format.o sort.o
DEP = $(OBJ:.o=.d)

all: myls

test: myls
	sh tests/smoke.sh

myls: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(DEP) myls

.PHONY: all clean test
-include $(DEP)
