CC := gcc
CFLAGS := -Wall -fPIC -I. -I../Utils
LIBNAME := client_stub

all: main clean_obj

main: main.c lib$(LIBNAME).so
	$(CC) $(CFLAGS) main.c -o main -L . -l$(LIBNAME)

lib$(LIBNAME).so: client_stub.o ../Utils/packet.o asincCall.o menu.o
	$(CC) -shared $(CFLAGS) client_stub.o ../Utils/packet.o asincCall.o menu.o -o lib$(LIBNAME).so

../Utils/packet.o: ../Utils/packet.c ../Utils/packet.h
	$(CC) $(CFLAGS) -c ../Utils/packet.c -o ../Utils/packet.o

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o *.so main
clean_obj:
	rm -f *.o

.PHONY: clean
