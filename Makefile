CC      = cc
CFLAGS  = -std=c11 -Wall -Wextra -O0 -g

OBJS = alloc.o main.o

all: main

main: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

alloc.o: alloc.c alloc.h
	$(CC) $(CFLAGS) -c alloc.c

main.o: main.c alloc.h
	$(CC) $(CFLAGS) -c main.c

clean:
	rm -f $(OBJS) main
