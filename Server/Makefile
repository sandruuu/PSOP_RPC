CC:= gcc
CFLAGS := -Wall -I../Utils

all: main clean_obj

main : main.c connectToClient.o ../Utils/packet.o functions.o queue.o ../Utils/log.o
	$(CC) $(CFLAGS) $^ -o $@

../Utils/packet.o: ../Utils/packet.c ../Utils/packet.h
	$(CC) $(CFLAGS) -c ../Utils/packet.c -o ../Utils/packet.o

../Utils/log.o: ../Utils/log.c ../Utils/log.h
	$(CC) $(CFLAGS) -DLOG_USE_COLOR -c ../Utils/log.c -o ../Utils/log.o

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean_obj:
	rm -f *.o ../Utils/packet.o ../Utils/log.o

clean:
	rm -f *.o

.PHONY: clean

