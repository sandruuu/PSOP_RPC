CC:= gcc
CFLAGS := -Wall -I../Utils
all: main clean_obj
main : FunctionLib
	$(CC) $@.c -o $@ -l$< -L .

FunctionLib : add.o connectToClient.o ../Utils/packet.o server_stub.o epollConnection.o asincCall.o
	$(CC) -shared $(CFLAGS) $^ -o lib$@.so

../Utils/packet.o: ../Utils/packet.c ../Utils/packet.h
	$(CC) $(CFLAGS) -c ../Utils/packet.c -o ../Utils/packet.o
%.o : %.c
	$(CC) -c $(CFLAGS) -fPIC $< -o $@

clean:
	rm -f *.o *.so main
clean_obj:
	rm -f *.o

.PHONY: clean