CC=gcc
CFLAGS=-c -Wall

all: dlmirror

dlmirror: tubecable.o helper.o screencap_flinger.o dlmirror.o
	$(CC) tubecable.o helper.o screencap_flinger.o dlmirror.o -lusb -lpthread -s -o dlmirror

tubecable.o: tubecable.c
	$(CC) $(CFLAGS) tubecable.c 

helper.o: helper.c
	$(CC) $(CFLAGS) helper.c

screencap_flinger.o: screencap_flinger.cpp
	$(CC) $(CFLAGS) screencap_flinger.cpp

dlmiror.o: dlmirror.c
	$(CC) $(CFLAGS) dlmirror.c

clean:
	rm -rf *.o dlmirror

run: dlmirror
	sudo ./dlmirror
