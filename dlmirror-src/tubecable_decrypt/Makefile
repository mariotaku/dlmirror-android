CC=gcc
CFLAGS=-c -Wall

all: dldecrypt

dldecrypt: tubecable.o dldecrypt.o
	$(CC) tubecable.o dldecrypt.o -lusb -s -o dldecrypt

tubecable.o: jni/tubecable.c
	$(CC) $(CFLAGS) jni/tubecable.c 

dldecrypt.o: jni/dldecrypt.c
	$(CC) $(CFLAGS) jni/dldecrypt.c

clean:
	rm -rf *.o dldecrypt

run: dlmirror
	sudo ./dldecrypt
