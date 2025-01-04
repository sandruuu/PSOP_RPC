CC := gcc
CFLAGS := -Wall -fPIC -I. -I../Utils
LIBNAME := client_stub

all: main main_async clean_obj

main: main.c lib$(LIBNAME).so
	$(CC) $(CFLAGS) main.c -o main -L . -l$(LIBNAME)

main_async: main_async.c lib$(LIBNAME).so
	$(CC) $(CFLAGS) main_async.c -o main_async -L . -l$(LIBNAME)

lib$(LIBNAME).so: client_stub.o ../Utils/packet.o
	$(CC) -shared $(CFLAGS) client_stub.o ../Utils/packet.o -o lib$(LIBNAME).so

../Utils/packet.o: ../Utils/packet.c ../Utils/packet.h
	$(CC) -fPIC -c ../Utils/packet.c -o ../Utils/packet.o

%.o: %.c
	$(CC) -fPIC -c $< -o $@

clean:
	rm -f *.o *.so main main_async

clean_obj:
	rm -f *.o ../Utils/*.o

.PHONY: clean