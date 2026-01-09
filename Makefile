CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror -O2

OBJS = main.o events.o slist.o queue.o stack.o station_index.o nary.o rules.o csv_loader.o json_loader.o

all: ev_demo

ev_demo: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) ev_demo
