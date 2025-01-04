CC := gcc
CFLAGS := -Wall -fPIC -I. -I../Utils
LIBNAME := client_stub

all: client1_async client2_async client3_async client4_async client5_async \
     client1_sync client2_sync client3_sync client4_sync client5_sync \
     clean_obj

%_async: %_async.c $(LIBRARY)
	$(CC) $(CFLAGS) $< -o $@ -L../Client -l$(LIBNAME)

%_sync: %_sync.c $(LIBRARY)
	$(CC) $(CFLAGS) $< -o $@ -L../Client -l$(LIBNAME)

%.o: %.c
	$(CC) -fPIC -c $< -o $@

clean:
	rm -f *_async *_sync

clean_obj:
	rm -f *.o

.PHONY: clean