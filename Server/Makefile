CC:= gcc
CFLAGS := -Wall -I../Utils

all: main clean_obj

main : main.c connectToClient.o ../Utils/packet.o functions.o Queue.o
	$(CC) $(CFLAGS) $^ -o $@

../Utils/packet.o: ../Utils/packet.c ../Utils/packet.h
	$(CC) $(CFLAGS) -c ../Utils/packet.c -o ../Utils/packet.o

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean_obj:
	rm -f *.o ../Utils/packet.o

clean:
	rm -f *.o

.PHONY: clean

