all: producer_consumer_system

CC = gcc
INCLUDE = ./include
CFLAGS = -Wall -std=c99 -pthread -DRB_IO

producer_consumer_system: cmd_args.o consumer.o main.o producer.o ring_buffer.o sleep_thread.o thread.o
	$(CC) -o producer_consumer_system_app cmd_args.o consumer.o main.o producer.o ring_buffer.o sleep_thread.o thread.o -pthread
cmd_args.o: src/cmd_args.c include/cmd_args.h
	$(CC) -I$(INCLUDE) $(CFLAGS) -c src/cmd_args.c
consumer.o: src/consumer.c include/consumer.h
	$(CC) -I$(INCLUDE) $(CFLAGS) -c src/consumer.c
main.o: src/main.c
	$(CC) -I$(INCLUDE) $(CFLAGS) -c src/main.c
producer.o: src/producer.c include/producer.h
	$(CC) -I$(INCLUDE) $(CFLAGS) -c src/producer.c
ring_buffer.o: src/ring_buffer.c include/ring_buffer.h
	$(CC) -I$(INCLUDE) $(CFLAGS) -c src/ring_buffer.c
sleep_thread.o: src/sleep_thread.c include/sleep_thread.h
	$(CC) -I$(INCLUDE) $(CFLAGS) -c src/sleep_thread.c
thread.o: src/thread.c include/thread.h
	$(CC) -I$(INCLUDE) $(CFLAGS) -c src/thread.c

.PHONY: clean

clean:
	rm -f *.o producer_consumer_system_app
