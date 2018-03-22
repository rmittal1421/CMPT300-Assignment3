CC = gcc
CFLAGS =
PROG = main
OBJS = list.o simulator.o main.o

run: $(OBJS)
	$(CC) $(CFLAGS) -o $(PROG) $(OBJS)

.c.o:
	$(CC) $(CFLAGS) -c $*.c

simulator.o: list.h

main.o: list.h simulator.h

clean:
	rm *.o the-simulator