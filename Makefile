CC=gcc
CFLAGS=-c -Wall

all: tubecable

tubecable: tubecable.o helper.o android_screencap.o tubecable_demo.o
	$(CC) tubecable.o helper.o android_screencap.o tubecable_demo.o -lusb -s -o bin/tubecable 

tubecable.o: jni/tubecable.c
	$(CC) $(CFLAGS) jni/tubecable.c 

helper.o: jni/helper.c
	$(CC) $(CFLAGS) jni/helper.c

android_screencap.o: jni/android_screencap.c
	$(CC) $(CFLAGS) jni/android_screencap.c

tubecable_demo.o: jni/tubecable_demo.c
	$(CC) $(CFLAGS) jni/tubecable_demo.c

clean:
	rm -rf *.o bin/tubecable

run: tubecable
	sudo bin/tubecable
