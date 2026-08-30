CC      = cc
CFLAGS  = -std=c11 -Wall -Wextra -D_DEFAULT_SOURCE -O0 -g

OBJS = test.o alloc.o main.o find.o

all: main

main: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

find.o: alloc.h find.c find.h
	$(CC) $(CFLAGS) -c find.c

alloc.o: alloc.c alloc.h
	$(CC) $(CFLAGS) -c alloc.c

test.o: test.c test.h
	$(CC) $(CFLAGS) -c test.c

main.o: main.c alloc.h test.h
	$(CC) $(CFLAGS) -c main.c

clean:
	rm -f $(OBJS) main
