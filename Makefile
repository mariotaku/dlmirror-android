CC=g++
CFLAGS=-c -Wall

all: tubecable

tubecable: tubecable.o helper.o android_screencap.o tubecable_demo.o
	$(CC) tubecable.o helper.o android_screencap.o tubecable_demo.o -lusb -s -o bin/tubecable 

tubecable.o: jni/tubecable.cpp
	$(CC) $(CFLAGS) jni/tubecable.cpp 

helper.o: jni/helper.cpp
	$(CC) $(CFLAGS) jni/helper.cpp

android_screencap.o: jni/android_screencap.cpp
	$(CC) $(CFLAGS) jni/android_screencap.cpp

tubecable_demo.o: jni/tubecable_demo.cpp
	$(CC) $(CFLAGS) jni/tubecable_demo.cpp

clean:
	rm  *.o bin/tubecable

run: tubecable
	sudo bin/tubecable
